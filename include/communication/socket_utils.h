//
// Created by Petio Petrov on 2024-10-03.
//

#pragma once

#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/fcntl.h>

#include <string>

#include "utils/macros.h"

namespace Communication {
    struct SocketCfg {
        std::string ip;
        std::string iface;
        int port = -1;
        bool is_udp = false;
        bool is_listening = false;
        bool needs_so_timestamp = false;
    };

    constexpr int MaxTCPServerBacklog = 1024;  // max pending TCP connections.

    inline auto getIfaceIP(const std::string &iface) -> std::string {
        char buf[NI_MAXHOST] = {'\0'};
        ifaddrs *ifaddr = nullptr;

        if (getifaddrs(&ifaddr) != -1) {
            for (ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET
                    && iface == ifa->ifa_name) {
                    getnameinfo(
                        ifa->ifa_addr,
                        sizeof(sockaddr_in),
                        buf,
                        sizeof(buf),
                        NULL,
                        0,
                        NI_NUMERICHOST
                    );
                    break;
                }
            }
            freeifaddrs(ifaddr);
        }

        return buf;
    }

    inline auto setNonBlocking(int fd) -> bool {
        const auto flags = fcntl(fd, F_GETFL, 0);
        if (flags & O_NONBLOCK) return true;
        return (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1);
    }

    inline auto disableNagle(int fd) -> bool {
        int one = 1;
        return (
            setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void *>(&one), sizeof(one))
            != -1
        );
    }

    inline auto setSOTimestamp(int fd) -> bool {
        int one = 1;
        return (
            setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void *>(&one), sizeof(one))
            != -1
        );
    }

    constexpr sockaddr_in createSockaddrIn(uint16_t port) {
#ifdef __APPLE__
        return sockaddr_in{0, AF_INET, htons(port), {htonl(INADDR_ANY)}, {0}};
#else
        return sockaddr_in{AF_INET, htons(port), {htonl(INADDR_ANY)}, {0}};
#endif
    }

    //    inline auto join(int fd, const std::string &ip) -> bool;

    [[nodiscard]] inline auto createSocket(const SocketCfg &socket_cfg) -> int {
        std::string time_str;

        const auto ip = socket_cfg.ip.empty() ? getIfaceIP(socket_cfg.iface) : socket_cfg.ip;

        const int input_flags
            = (socket_cfg.is_listening ? AI_PASSIVE : 0) | (AI_NUMERICHOST | AI_NUMERICSERV);
        const addrinfo hints{
            input_flags,
            AF_INET,
            socket_cfg.is_udp ? SOCK_DGRAM : SOCK_STREAM,
            socket_cfg.is_udp ? IPPROTO_UDP : IPPROTO_TCP,
            0,
            0,
            nullptr,
            nullptr
        };

        addrinfo *result = nullptr;
        const auto rc
            = getaddrinfo(ip.c_str(), std::to_string(socket_cfg.port).c_str(), &hints, &result);
        ASSERT(
            !rc,
            "getaddrinfo() failed. error:" + std::string(gai_strerror(rc))
                + "errno:" + strerror(errno)
        );

        int socket_fd = -1;
        int one = 1;
        for (addrinfo *rp = result; rp; rp = rp->ai_next) {
            ASSERT(
                (socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) != -1,
                "socket() failed. errno:" + std::string(strerror(errno))
            );

            ASSERT(
                setNonBlocking(socket_fd),
                "setNonBlocking() failed. errno:" + std::string(strerror(errno))
            );

            if (!socket_cfg.is_udp) {  // disable Nagle for TCP sockets.
                ASSERT(
                    disableNagle(socket_fd),
                    "disableNagle() failed. errno:" + std::string(strerror(errno))
                );
            }

            if (!socket_cfg.is_listening) {  // establish connection to specified address.
                ASSERT(
                    connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != 1,
                    "connect() failed. errno:" + std::string(strerror(errno))
                );
            }

            if (socket_cfg.is_listening) {  // allow re-using the address in the call to bind()
                ASSERT(
                    setsockopt(
                        socket_fd,
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        reinterpret_cast<const char *>(&one),
                        sizeof(one)
                    ) == 0,
                    "setsockopt() SO_REUSEADDR failed. errno:" + std::string(strerror(errno))
                );
            }

            if (socket_cfg.is_listening) {
                // bind to the specified port number.
                const sockaddr_in addr = createSockaddrIn(socket_cfg.port);
                ASSERT(
                    bind(
                        socket_fd,
                        socket_cfg.is_udp ? reinterpret_cast<const struct sockaddr *>(&addr)
                                          : rp->ai_addr,
                        sizeof(addr)
                    ) == 0,
                    "bind() failed. errno:%" + std::string(strerror(errno))
                );
            }

            if (!socket_cfg.is_udp
                && socket_cfg.is_listening) {  // listen for incoming TCP connections.
                ASSERT(
                    listen(socket_fd, MaxTCPServerBacklog) == 0,
                    "listen() failed. errno:" + std::string(strerror(errno))
                );
            }

            if (socket_cfg.needs_so_timestamp) {  // enable software receive timestamps.
                ASSERT(
                    setSOTimestamp(socket_fd),
                    "setSOTimestamp() failed. errno:" + std::string(strerror(errno))
                );
            }
            break;
        }

        return socket_fd;
    }

}  // namespace Communication
