/**============================================================================
Name        : Utilities.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Utilities
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

#include <x86intrin.h>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <cstdio>
#include <fcntl.h>
#include <cstring>
#include <filesystem>

#include <liburing.h>

#define BUFFER_SIZE 4096
#define QUEUE_DEPTH 1

namespace
{
    constexpr std::filesystem::path currentDir() noexcept
    {
        return std::filesystem::current_path() / "../../resources";
    }
}

void readFile()
{
    io_uring ring{};
    io_uring_cqe *cqe;   // Completion Queue Entry
    char buf[BUFFER_SIZE];

    // 1. Spin up the ring — depth 1 means 1 in-flight op at a time
    int ret = io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
    if (ret < 0) {
        std::cerr << "Queue init failed: " << strerror(-ret) << std::endl;
        return;
    }

    // 2. Open file (still a regular syscall — totally fine here)
    const std::filesystem::path filePath = currentDir() / "test_file.txt";
    const int fd = ::open(filePath.string().data(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    // 3. Grab a free slot in the SQ
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);

    // 4. Describe the operation — read from fd into buf at offset 0
    io_uring_prep_read(sqe, fd, buf, BUFFER_SIZE, 0);

    // 5. Tag it — this comes back in cqe->user_data so you know what finished
    io_uring_sqe_set_data(sqe, buf);

    // 6. Submit — THE one syscall. Everything queued goes in one shot.
    ret = io_uring_submit(&ring);
    if (ret < 0) {
        fprintf(stderr, "Submit failed: %s\n", strerror(-ret));
        return;
    }

    // 7. Block until at least one CQE is available
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
        fprintf(stderr, "Wait failed: %s\n", strerror(-ret));
        return;
    }

    // 8. cqe->res = bytes read on success, negative errno on failure
    if (cqe->res < 0) {
        fprintf(stderr, "Read error: %s\n", strerror(-cqe->res));
    } else {
        buf[cqe->res] = '\0';
        printf("Read %d bytes:\n%s\n", cqe->res, buf);
    }

    // 9. Tell the kernel you've consumed this CQE — advances the CQ head
    io_uring_cqe_seen(&ring, cqe);

    ::close(fd);
    io_uring_queue_exit(&ring);

}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    readFile();

    return EXIT_SUCCESS;
}
