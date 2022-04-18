#include <anet/udp/udp_server.hpp>
#include <iostream>

using anet::udp::UdpConnectionPtr;
using anet::udp::UdpServer;

int main() {
  asio::io_context io_context;
  UdpServer server(io_context);
  server.SetConnReadCallback([](const UdpConnectionPtr &conn, std::string_view data) {
    std::cout << "Received: " << data << std::endl;
    conn->Send(data);
    conn->DoReceive();
  });
  server.Listen("localhost", "9987");
  io_context.run();
}
