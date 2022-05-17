#include <anet/tcp/tcp_client.hpp>
#include <iostream>

using namespace std::chrono_literals;

int main() {
  asio::io_context io_context;
  anet::tcp::TcpClient client(io_context, "localhost", "9987");
  client.SetConnectTimeout(10s);
  client.SetNewConnCallback([](const anet::tcp::TcpConnectionPtr &conn) {
    std::cout << "Connect to " << conn->GetRemoteEndpoint() << std::endl;
    conn->Send("Hello");
//    conn->Send("Hello, world");
  });
  client.SetConnWriteCallback([](const anet::tcp::TcpConnectionPtr &conn) {
    conn->DoRead();
  });
  client.SetConnReadCallback([](const anet::tcp::TcpConnectionPtr &conn, std::string_view data) {
    std::cout << "Received: " << data << std::endl;
    conn->DoRead();
  });
  client.SetConnCloseCallback([](anet::tcp::TcpConnection *conn, const anet::tcp::Tcp::endpoint &remote_endpoint) {
    std::cout << "closed" << std::endl;
  });
  client.StartConnect();
  io_context.run();
}
