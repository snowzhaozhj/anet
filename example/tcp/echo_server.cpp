#include <anet/tcp/tcp_server.hpp>
#include <iostream>

using anet::tcp::TcpServer;
using anet::tcp::TcpConnectionPtr;

int main() {
  TcpServer server(1);
  server.SetNewConnCallback([](const TcpConnectionPtr &conn) {
    conn->DoRead();
  });
  server.SetConnReadCallback([](const TcpConnectionPtr &conn, std::string_view data) {
    conn->Send(std::string(data));
    conn->DoRead();
  });
  server.Listen("localhost", "9987");
  server.Run();
}
