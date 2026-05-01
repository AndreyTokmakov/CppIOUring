#include "Networking.hpp"

#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

using namespace std::string_view_literals;

constexpr int PORT = 8080;
constexpr int QUEUE_DEPTH = 1024;
constexpr int BUFFER_SIZE = 4096;

constexpr std::string_view RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 13\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "Hello, world!"sv;


struct Connection
{
    int fd;
    char buffer[BUFFER_SIZE];
    size_t read_len = 0;
};

enum class OpType
{
    ACCEPT,
    READ,
    WRITE
};

struct Request
{
    OpType type;
    Connection* conn;
};

static int set_nonblocking(const int fd)
{
    const int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void add_accept(io_uring& ring, const int server_fd)
{
    auto* req = new Request{OpType::ACCEPT, nullptr};
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, server_fd, nullptr, nullptr, 0);
    sqe->user_data = reinterpret_cast<uint64_t>(req);
}

void add_read(io_uring& ring, Connection* conn)
{
    auto* req = new Request{OpType::READ, conn};
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_recv(sqe, conn->fd, conn->buffer, BUFFER_SIZE, 0);
    sqe->user_data = reinterpret_cast<uint64_t>(req);
}

void add_write(io_uring& ring, Connection* conn, const char* data, const size_t len)
{
    auto* req = new Request{OpType::WRITE, conn};
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_send(sqe, conn->fd, data, len, 0);
    sqe->user_data = reinterpret_cast<uint64_t>(req);
}

[[noreturn]]
void runServer()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
    set_nonblocking(server_fd);

    io_uring ring;
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    add_accept(ring, server_fd);
    io_uring_submit(&ring);

    while (true)
    {
        io_uring_cqe* cqe;
        io_uring_wait_cqe(&ring, &cqe);
        const Request* req = reinterpret_cast<Request *>(cqe->user_data);
        if (cqe->res < 0) {
            std::cerr << "Error: " << strerror(-cqe->res) << "\n";
            delete req;
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        switch (req->type)
        {
            case OpType::ACCEPT:
            {
                const int client_fd = cqe->res;
                set_nonblocking(client_fd);
                auto* conn = new Connection{client_fd};
                add_read(ring, conn);
                add_accept(ring, server_fd);
                break;
            }
            case OpType::READ:
            {
                Connection* conn = req->conn;
                if (cqe->res == 0) {
                    close(conn->fd);
                    delete conn;
                    break;
                }
                add_write(ring, conn, RESPONSE.data(), RESPONSE.size());
                break;
            }
            case OpType::WRITE:
            {
                Connection* conn = req->conn;
                // keep-alive: read next request
                add_read(ring, conn);
                break;
            }
        }

        delete req;
        io_uring_cqe_seen(&ring, cqe);
        io_uring_submit(&ring);
    }

    io_uring_queue_exit(&ring);
    close(server_fd);
}

void Networking::HttpServer::TestAll()
{
    runServer();
}