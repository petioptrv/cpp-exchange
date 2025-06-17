#pragma once

#include <sys/event.h> // For kqueue
#include <sys/time.h>  // For kevent timeout
#include <unistd.h>    // For close

#include "tcp_socket.h"
#include "utils/time_utils.h"

namespace Utils {
    struct TCPServer {
        explicit TCPServer() {}

        /// Add and remove socket file descriptors to and from the kqueue list.
        auto addToKqueueList(TCPSocket *socket, int16_t filter, uint16_t flags) {
            struct kevent ev;
            EV_SET(&ev, socket->socket_fd_, filter, flags, 0, 0, socket);
            return kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) != -1;
        }

        /// Start listening for connections on the provided interface and port.
        auto listen(const std::string &iface, int port) -> void {
            kqueue_fd_ = kqueue();
            ASSERT(kqueue_fd_ >= 0, "kqueue() failed: " + std::string(std::strerror(errno)));

            ASSERT(listener_socket_.connect({"", iface, port, false, true, true}) >= 0,
                   "Listener socket failed to connect. iface:" + iface + " port:" + std::to_string(port) +
                       " error:" + std::string(std::strerror(errno)));

            ASSERT(addToKqueueList(&listener_socket_, EVFILT_READ, EV_ADD | EV_ENABLE),
                   "kevent() failed to add listener socket. error:" + std::string(std::strerror(errno)));
        }

        /// Check for new connections or dead connections and update containers that track the sockets.
        auto poll() noexcept -> void {
            const int max_events = 1 + send_sockets_.size() + receive_sockets_.size();
            struct kevent events[max_events];

            struct timespec timeout{0, 0};
            const int n = kevent(kqueue_fd_, nullptr, 0, events, max_events, &timeout);
            bool have_new_connection = false;

            for (int i = 0; i < n; ++i) {
                const auto &event = events[i];
                auto socket = reinterpret_cast<TCPSocket *>(event.udata);

                if (event.filter == EVFILT_READ) {
                    if (socket == &listener_socket_) {
                        have_new_connection = true;
                        continue;
                    }
                    if (std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) == receive_sockets_.end())
                        receive_sockets_.push_back(socket);
                }

                if (event.filter == EVFILT_WRITE) {
                    if (std::find(send_sockets_.begin(), send_sockets_.end(), socket) == send_sockets_.end())
                        send_sockets_.push_back(socket);
                }

                if (event.flags & (EV_ERROR | EV_EOF)) {
                    receive_sockets_.erase(std::remove(receive_sockets_.begin(), receive_sockets_.end(), socket),
                                           receive_sockets_.end());
                    send_sockets_.erase(std::remove(send_sockets_.begin(), send_sockets_.end(), socket),
                                        send_sockets_.end());
                    delete socket;
                }
            }

            while (have_new_connection) {
                int fd = accept(listener_socket_.socket_fd_, nullptr, nullptr);
                if (fd == -1)
                    break;

                ASSERT(Communication::setNonBlocking(fd) && Communication::disableNagle(fd),
                       "Failed to set non-blocking or no-delay on socket:" + std::to_string(fd));

                auto socket = new TCPSocket();
                socket->socket_fd_ = fd;
                socket->recv_callback_ = recv_callback_;
                ASSERT(addToKqueueList(socket, EVFILT_READ, EV_ADD | EV_ENABLE),
                       "Unable to add socket to kqueue. error:" + std::string(std::strerror(errno)));

                receive_sockets_.push_back(socket);
            }
        }

        /// Publish outgoing data from the send buffer and read incoming data from the receive buffer.
        auto sendAndRecv() noexcept -> void {
            bool recv = false;

            for (auto socket : receive_sockets_) {
                recv |= socket->sendAndRecv();
            }

            if (recv) // There were some events and they have all been dispatched, inform listener.
                recv_finished_callback_();

            for (auto socket : send_sockets_) {
                socket->sendAndRecv();
            }
        }

        ~TCPServer() {
            if (kqueue_fd_ != -1) {
                close(kqueue_fd_);
            }
        }

        /// Socket on which this server is listening for new connections on.
        int kqueue_fd_ = -1;
        TCPSocket listener_socket_;

        /// Collection of all sockets, sockets for incoming data, sockets for outgoing data and dead connections.
        std::vector<TCPSocket *> receive_sockets_, send_sockets_;

        /// Function wrapper to call back when data is available.
        std::function<void(TCPSocket *s, Utils::NsTimestampT rx_time)> recv_callback_ = nullptr;
        /// Function wrapper to call back when all data across all TCPSockets has been read and dispatched this round.
        std::function<void()> recv_finished_callback_ = nullptr;

        std::string time_str_;
    };
}  // namespace Common
