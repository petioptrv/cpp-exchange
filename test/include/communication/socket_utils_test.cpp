//
// Created by Petio Petrov on 2024-10-03.
//

#include "communication/socket_utils.h"

#include <doctest/doctest.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <thread>

#include <string>

using namespace std;
using namespace Communication;

TEST_CASE("Get interface IP") {
    CHECK((
        getIfaceIP("lo0") == "127.0.0.1"    // MacOS
        || getIfaceIP("lo") == "127.0.0.1"  // Linux
    ));
}

TEST_CASE("Set non-blocking socket") {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int current_flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, current_flags & ~O_NONBLOCK);

    CHECK(setNonBlocking(fd));

    current_flags = fcntl(fd, F_GETFL);

    CHECK((current_flags & O_NONBLOCK));
}

TEST_CASE("Disabling Nagle's algorithm") {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int enable = 0;
    int destination;
    socklen_t destination_len = sizeof(destination);

    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void *>(&enable), sizeof(enable));
    getsockopt(
        fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void *>(&destination), &destination_len
    );

    CHECK(destination == 0);
    CHECK(disableNagle(fd));

    getsockopt(
        fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void *>(&destination), &destination_len
    );

    CHECK(destination != 0);
}

TEST_CASE("Set software receive timestamps for network packets") {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int enable = 0;
    int destination;
    socklen_t destination_len = sizeof(destination);

    setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void *>(&enable), sizeof(enable));
    getsockopt(
        fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void *>(&destination), &destination_len
    );

    CHECK(destination == 0);
    CHECK(setSOTimestamp(fd));

    getsockopt(
        fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void *>(&destination), &destination_len
    );

    CHECK(destination != 0);
}
