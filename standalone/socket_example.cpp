#include <thread>
#include <chrono>
#include "utils/time_utils.h"
#include "communication/tcp_server.h"

int main(int, char **) {
  using namespace Common;

  std::string time_str_;

  auto tcpServerRecvCallback = [&](TCPSocket *socket, NsTimestampT rx_time) noexcept {
    printf("TCPServer::defaultRecvCallback() socket:%zu len:%zu rx:%lld\n",
                socket->socket_fd_, socket->next_rcv_valid_index_, rx_time);

    const std::string reply = "TCPServer received msg:" + std::string(socket->inbound_data_.data(), socket->next_rcv_valid_index_);
    socket->next_rcv_valid_index_ = 0;

    socket->send(reply.data(), reply.length());
  };

  auto tcpServerRecvFinishedCallback = [&]() noexcept {
    printf("TCPServer::defaultRecvFinishedCallback()\n");
  };

  auto tcpClientRecvCallback = [&](TCPSocket *socket, NsTimestampT rx_time) noexcept {
    const std::string recv_msg = std::string(socket->inbound_data_.data(), socket->next_rcv_valid_index_);
    socket->next_rcv_valid_index_ = 0;

    printf("TCPSocket::defaultRecvCallback() socket:%zu len:%zu rx:%zu msg:%s\n",
                socket->socket_fd_, socket->next_rcv_valid_index_, rx_time, recv_msg.c_str());
  };

  const std::string iface = "lo";
  const std::string ip = "127.0.0.1";
  const int port = 12345;

  printf("Creating TCPServer on iface:%s port:%zu\n", iface.c_str(), port);
  TCPServer server;
  server.recv_callback_ = tcpServerRecvCallback;
  server.recv_finished_callback_ = tcpServerRecvFinishedCallback;
  server.listen(iface, port);

  std::vector<TCPSocket *> clients(2);

  for (size_t i = 0; i < clients.size(); ++i) {
    clients[i] = new TCPSocket;
    clients[i]->recv_callback_ = tcpClientRecvCallback;

    printf("Connecting TCPClient-[%zu] on ip:%s iface:%s port:%zu\n", i, ip.c_str(), iface.c_str(), port);
    clients[i]->connect({ip, iface, port, false, false, true});
    server.poll();
  }

  using namespace std::literals::chrono_literals;

  for (auto itr = 0; itr < 5; ++itr) {
    for (size_t i = 0; i < clients.size(); ++i) {
      const std::string client_msg = "CLIENT-[" + std::to_string(i) + "] : Sending " + std::to_string(itr * 100 + i);
      printf("Sending TCPClient-[%zu] %s\n", i, client_msg.c_str());
      clients[i]->send(client_msg.data(), client_msg.length());
      clients[i]->sendAndRecv();

      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      server.poll();
      server.sendAndRecv();
    }
  }

  return 0;
}
