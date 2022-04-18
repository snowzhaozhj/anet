#include <anet/udp/udp_client.hpp>
#include <iostream>

using anet::udp::UdpClient;
using anet::udp::UdpConnection;
using anet::udp::UdpConnectionPtr;

int main() {
  asio::io_context io_context;
  UdpClient client(io_context);
  client.SetConnCloseCallback([](const UdpConnection *conn) {
    std::cout << "closed" << std::endl;
  });
  client.SetConnReadCallback([](const UdpConnectionPtr &conn, std::string_view s) {
    std::cout << s << std::endl;
  });
  client.Connect("localhost", "9987", [](const UdpConnectionPtr &conn) {
    conn->Send("Hello, world!");
    conn->DoReceive();
  });
  io_context.run();
}
