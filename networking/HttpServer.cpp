#include "Networking.hpp"

#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <memory>
#include <iostream>

using namespace std::string_view_literals;


namespace
{
    using Handle = int32_t;
    using Port   = uint16_t;

    constexpr Handle InvalidHandle { -1 };
    constexpr Port serverPort { 52525 };
    constexpr uint32_t QueueDepth { 1024 };
    constexpr uint32_t BufferSize { 4096 };

    constexpr std::string_view RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 13\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "Hello, world!"sv;

    struct Connection
    {
        Handle fd { InvalidHandle };
        std::array<char, BufferSize> buffer{};
    };

    enum class OpType: uint8_t
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

    struct SocketGuard final
    {
        Handle sock { InvalidHandle };

        explicit SocketGuard(const Handle s): sock {s} { }

        ~SocketGuard()
        {
            if (sock != InvalidHandle) {
                ::close(sock);
            }
        }

        SocketGuard(const SocketGuard&) = delete;
        SocketGuard(SocketGuard&&) noexcept = delete;

        SocketGuard& operator=(const SocketGuard&) = delete;
        SocketGuard& operator=(SocketGuard&&) noexcept = delete;
    };

    Handle seNonBlocking(const Handle sock)
    {
        const Handle flags = ::fcntl(sock, F_GETFL, 0);
        if (flags == InvalidHandle) {
            throw std::runtime_error("fcntl(F_GETFL) failed");
        }
        if (const Handle handle = ::fcntl(sock, F_SETFL, flags | O_NONBLOCK); handle == InvalidHandle) {
            throw std::runtime_error ( "::fcntl() failed" );
        } else {
            return handle;
        }
    }

    void addAccept(io_uring& ring, const Handle server_fd)
    {
        Request* req = new Request{OpType::ACCEPT, nullptr};
        io_uring_sqe* sqe = ::io_uring_get_sqe(&ring);
        ::io_uring_prep_accept(sqe, server_fd, nullptr, nullptr, 0);
        sqe->user_data = reinterpret_cast<uint64_t>(req);
    }

    void addRead(io_uring& ring, Connection* conn)
    {
        Request* req = new Request{OpType::READ, conn};
        io_uring_sqe* sqe = ::io_uring_get_sqe(&ring);
        ::io_uring_prep_recv(sqe, conn->fd, std::data(conn->buffer), std::size(conn->buffer), 0);
        sqe->user_data = reinterpret_cast<uint64_t>(req);
    }

    void addWrite(io_uring& ring, Connection* conn, const char* data, const size_t len)
    {
        Request* req = new Request{OpType::WRITE, conn};
        io_uring_sqe* sqe = ::io_uring_get_sqe(&ring);
        ::io_uring_prep_send(sqe, conn->fd, data, len, 0);
        sqe->user_data = reinterpret_cast<uint64_t>(req);
    }
}

[[noreturn]]
void runServer()
{
    const Handle serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        throw std::runtime_error("socket failed");
    }

    SocketGuard socketGuard(serverFd);
    const sockaddr_in server { AF_INET, htons(serverPort), {.s_addr = INADDR_ANY}, {}};

    if (::bind(serverFd, reinterpret_cast<const sockaddr*>(&server),sizeof(server)) < 0) {
        throw std::runtime_error("bind failed");
    }

    listen(serverFd, SOMAXCONN);
    seNonBlocking(serverFd);

    io_uring ring{};
    :: io_uring_queue_init(QueueDepth, &ring, 0);

    addAccept(ring, serverFd);
    :: io_uring_submit(&ring);

    while (true)
    {
        io_uring_cqe* cqe;
        ::io_uring_wait_cqe(&ring, &cqe);
        std::unique_ptr<Request> request {reinterpret_cast<Request*>(cqe->user_data) };
        if (cqe->res < 0) {
            std::cerr << "Error: " << strerror(-cqe->res) << "\n";
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        switch (request->type)
        {
            case OpType::ACCEPT:
            {
                const int client_fd = cqe->res;
                seNonBlocking(client_fd);
                auto* conn = new Connection{client_fd};
                addRead(ring, conn);
                addAccept(ring, serverFd);
                break;
            }
            case OpType::READ:
            {
                Connection* conn = request->conn;
                if (cqe->res == 0) {
                    close(conn->fd);
                    delete conn;
                    break;
                }
                addWrite(ring, conn, RESPONSE.data(), RESPONSE.size());
                break;
            }
            case OpType::WRITE:
            {
                Connection* conn = request->conn;
                // keep-alive: read next request
                addRead(ring, conn);
                break;
            }
        }

        io_uring_cqe_seen(&ring, cqe);
        io_uring_submit(&ring);
    }

    ::io_uring_queue_exit(&ring);
}

void Networking::HttpServer::TestAll()
{
    runServer();

    // http://127.0.0.1:52525/
}