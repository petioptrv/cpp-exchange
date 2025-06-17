//
// Created by Petio Petrov on 2024-10-13.
//

#pragma once

#include <functional>

#include "socket_utils.h"
#include "utils/time_utils.h"

namespace Common {
    /// Size of our send and receive buffers in bytes.
    constexpr size_t TCPBufferSize = 64 * 1024 * 1024;

    struct TCPSocket {
        explicit TCPSocket() {
            outbound_data_.resize(TCPBufferSize);
            inbound_data_.resize(TCPBufferSize);
        }

        /// Deleted default, copy & move constructors and assignment-operators.
        TCPSocket(const TCPSocket &) = delete;

        TCPSocket(const TCPSocket &&) = delete;

        TCPSocket &operator=(const TCPSocket &) = delete;

        TCPSocket &operator=(const TCPSocket &&) = delete;

        /// Create TCPSocket with provided attributes to either listen-on / connect-to.
        auto connect(const Communication::SocketCfg& socket_cfg) -> int {
            // Note that needs_so_timestamp=true for FIFOSequencer.
            socket_fd_ = createSocket(socket_cfg);

            socket_attrib_.sin_addr.s_addr = INADDR_ANY;
            socket_attrib_.sin_port = htons(socket_cfg.port);
            socket_attrib_.sin_family = AF_INET;

            return socket_fd_;
        }

        /// Called to publish outgoing data from the buffers as well as check for and callback if
        /// data is available in the read buffers.
        auto sendAndRecv() noexcept -> bool {
            char ctrl[CMSG_SPACE(sizeof(struct timeval))];
            auto cmsg = reinterpret_cast<struct cmsghdr *>(&ctrl);

            iovec iov{
                inbound_data_.data() + next_rcv_valid_index_, TCPBufferSize - next_rcv_valid_index_
            };
            msghdr msg{&socket_attrib_, sizeof(socket_attrib_), &iov, 1, ctrl, sizeof(ctrl), 0};

            // Non-blocking call to read available data.
            const auto read_size = recvmsg(socket_fd_, &msg, MSG_DONTWAIT);
            if (read_size > 0) {
                next_rcv_valid_index_ += read_size;

                Common::NsTimestampT kernel_time = 0;
                timeval time_kernel;
                if (cmsg->cmsg_level == SOL_SOCKET &&
                    cmsg->cmsg_type == SCM_TIMESTAMP &&
                    cmsg->cmsg_len == CMSG_LEN(sizeof(time_kernel))) {
                    memcpy(&time_kernel, CMSG_DATA(cmsg), sizeof(time_kernel));
                    kernel_time = time_kernel.tv_sec * NANOS_TO_SECS
                                  + time_kernel.tv_usec
                                        * NANOS_TO_MICROS;  // convert timestamp to nanoseconds.
                }

                const auto user_time = getCurrentNsTimestamp();

                recv_callback_(this, kernel_time);
            }

            if (next_send_valid_index_ > 0) {
                // Non-blocking call to send data.
                const auto n = ::send(
                    socket_fd_,
                    outbound_data_.data(),
                    next_send_valid_index_,
                    MSG_DONTWAIT | MSG_NOSIGNAL
                );
            }
            next_send_valid_index_ = 0;

            return (read_size > 0);
        }

        /// Write outgoing data to the send buffers.
        auto send(const void *data, size_t len) noexcept -> void {
            memcpy(outbound_data_.data() + next_send_valid_index_, data, len);
            next_send_valid_index_ += len;
        }

        /// File descriptor for the socket.
        int socket_fd_ = -1;

        /// Send and receive buffers and trackers for read/write indices.
        std::vector<char> outbound_data_;
        size_t next_send_valid_index_ = 0;
        std::vector<char> inbound_data_;
        size_t next_rcv_valid_index_ = 0;

        /// Socket attributes.
        struct sockaddr_in socket_attrib_{};

        /// Function wrapper to callback when there is data to be processed.
        std::function<void(TCPSocket *s, Common::NsTimestampT rx_time)> recv_callback_ = nullptr;

        std::string time_str_;
    };
}  // namespace Common
