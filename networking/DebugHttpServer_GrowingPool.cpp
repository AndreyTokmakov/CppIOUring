#include "Networking.hpp"
#include "DateTimeUtilities.hpp"

#include <iostream>
#include <print>
#include <syncstream>
#include <memory>
#include <numeric>

#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using namespace std::string_view_literals;

#define LOG std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << ' '
#define ERR std::osyncstream { std::cerr } << DateTimeUtilities::getCurrentTime() << ' '

namespace
{
    using Handle = int32_t;
    using Port   = uint16_t;

    constexpr Handle InvalidHandle { -1 };
    constexpr Port serverPort { 52525 };
    constexpr uint32_t QueueDepth { 1024 };
    constexpr uint32_t BufferSize { 1024 * 4 };

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
        // uint32_t readLen { 0U };
        // uint32_t writeOffset { 0U };
    };

    enum class OpType: uint8_t
    {
        Accept,
        Read,
        Write
    };

    struct Request
    {
        OpType type;
        Connection* conn { nullptr };
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

    template <typename Ty, typename Allocator = std::allocator<Ty>>
    struct ObjectPoolGrowing
    {
        using object_type = Ty;
        using pointer     = object_type*;
        using size_type   = std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        static constexpr size_type DefaultChunkSize { 64 };

        std::vector<pointer> pool;
        std::vector<pointer> available;

        Allocator allocator {};

        ObjectPoolGrowing() {
            addChunk(DefaultChunkSize);
        }

        pointer alloc()
        {
            if (available.empty()) {
                addChunk(DefaultChunkSize);
            }
            const pointer obj = available.back();
            available.pop_back();
            LOG << "[alloc] Size " << available.size() + 1 << " ==> " << available.size() << std::endl;
            return obj;
        }

        void free(pointer obj)
        {
            available.push_back(obj);
            LOG << "[free] Size " << available.size() - 1 << " ==> " << available.size() << std::endl;
        }

        ~ObjectPoolGrowing()
        {
            // Deallocate all allocated memory.
            for (pointer chunk : pool) {
                allocator.deallocate(chunk, DefaultChunkSize);
            }
        }

    private:

        void addChunk(const size_type chunkSize)
        {
            // Allocate a new chunk of uninitialized memory
            pointer newBlock { allocator.allocate(chunkSize) };

            // Keep all allocated blocks in 'pool' to delete them later:
            pool.push_back(newBlock);

            const size_t oldSize = available.size();
            available.resize(oldSize + chunkSize);
            std::iota(std::begin(available) + oldSize, std::end(available), newBlock);
        }
    };

    ObjectPoolGrowing<Connection> connPool;
    ObjectPoolGrowing<Request> reqPool;

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
        Request* req = reqPool.alloc();
        {
            req->type = OpType::Accept;
            req->conn = nullptr;
        }
        io_uring_sqe* sqe = ::io_uring_get_sqe(&ring);
        ::io_uring_prep_accept(sqe, server_fd, nullptr, nullptr, 0);
        sqe->user_data = reinterpret_cast<uint64_t>(req);
    }

    void addRead(io_uring& ring, Connection* conn)
    {
        Request* req = reqPool.alloc();
        {
            req->type = OpType::Read;
            req->conn = conn;
        }
        io_uring_sqe* sqe = ::io_uring_get_sqe(&ring);
        ::io_uring_prep_recv(sqe, conn->fd, std::data(conn->buffer), std::size(conn->buffer), 0);
        sqe->user_data = reinterpret_cast<uint64_t>(req);
    }

    void addWrite(io_uring& ring, Connection* conn, const char* data, const size_t len)
    {
        Request* req = reqPool.alloc();
        {
            req->type = OpType::Write;
            req->conn = conn;
        }
        io_uring_sqe* sqe = ::io_uring_get_sqe(&ring);
        ::io_uring_prep_send(sqe, conn->fd, data, len, 0);
        sqe->user_data = reinterpret_cast<uint64_t>(req);
    }
}

namespace Networking::DebugHttpServer_GrowingPool
{
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
            io_uring_cqe* cqe { nullptr };
            ::io_uring_wait_cqe(&ring, &cqe);
            Request* request = reinterpret_cast<Request*>(cqe->user_data);
            if (cqe->res < 0) {
                ERR << "Error: " << strerror(-cqe->res) << "\n";
                reqPool.free(request);
                ::io_uring_cqe_seen(&ring, cqe);
                continue;
            }

            switch (request->type)
            {
                case OpType::Accept:
                {
                    const int client_fd = cqe->res;
                    seNonBlocking(client_fd);
                    Connection* conn = connPool.alloc();
                    {
                        conn->fd = client_fd;
                        //conn->readLen = 0;
                        //conn->writeOffset = 0;
                    }
                    addAccept(ring, serverFd);
                    addRead(ring, conn);
                    break;
                }
                case OpType::Read:
                {
                    Connection* conn = request->conn;
                    if (cqe->res == 0)
                    {
                        ::close(conn->fd);
                        LOG << std::format("Closing connection {}\n", conn->fd) ;
                        connPool.free(conn);
                        break;
                    }
                    addWrite(ring, conn, RESPONSE.data(), RESPONSE.size());
                    break;
                }
                case OpType::Write:
                {
                    Connection* conn = request->conn;
                    // keep-alive: read next request
                    addRead(ring, conn);
                    break;
                }
            }

            reqPool.free(request);
            ::io_uring_cqe_seen(&ring, cqe);
            ::io_uring_submit(&ring);
        }

        ::io_uring_queue_exit(&ring);
    }
}

void Networking::DebugHttpServer_GrowingPool::TestAll()
{
    runServer();
    // http://127.0.0.1:52525/
}