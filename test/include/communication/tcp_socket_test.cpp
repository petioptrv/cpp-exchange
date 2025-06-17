//
// Created by Petio Petrov on 2025-06-17.
//

#include "communication/socket_utils.h"

#include <doctest/doctest.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <thread>

#include <string>

#include "communication/tcp_socket.h"


TEST_CASE("Create socket, send and receive") {
    const std::string ip = "127.0.0.1";
    const std::string iface = "lo";
    constexpr int port = 12345;

    const Communication::SocketCfg server_cfg{"", iface, port, false, true};
    const Communication::SocketCfg client_cfg{ip, iface, port, false, false};

    Utils::TCPSocket server_listener_socket;
    Utils::TCPSocket server_receiver_socket;
    Utils::TCPSocket client_socket;
    int received_value;

    server_receiver_socket.recv_callback_ = [&](Utils::TCPSocket* s, Utils::NsTimestampT) {
        int val = 0;
        memcpy(&val, s->inbound_data_.data(), sizeof(int));
        received_value = val;
        s->next_rcv_valid_index_ = 0;
    };

    ASSERT(server_listener_socket.connect(server_cfg) >= 0, "server connect() failed"); // binds and listens

    ASSERT(client_socket.connect(client_cfg) >= 0, "client connect() failed");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int fd = accept(server_listener_socket.socket_fd_, nullptr, nullptr);
    ASSERT(fd != -1, "accept() failed");
    server_receiver_socket.socket_fd_ = fd;

    int sent_value = 1234;
    client_socket.send(&sent_value, sizeof(sent_value));
    client_socket.sendAndRecv();

    // Let the server read
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    server_receiver_socket.sendAndRecv();

    REQUIRE(received_value == sent_value);
}