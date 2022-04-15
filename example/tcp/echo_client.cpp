#include <anet/tcp/tcp_client.hpp>
#include <iostream>

using namespace std::chrono_literals;

int main() {
  asio::io_context io_context;
  anet::tcp::TcpClient client(io_context, "www.baidu.com", "80");
//  client.SetConnectTimeout(10s);
  client.SetNewConnCallback([](const anet::tcp::TcpConnectionPtr &conn) {
    std::cout << "Connect to " << conn->GetRemoteEndpoint() << std::endl;
    conn->Send("GET / HTTP/1.1\r\n\r\n");
//    conn->Send("Hello, world");
  });
  client.SetConnWriteCallback([](const anet::tcp::TcpConnectionPtr &conn) {
    conn->DoRead();
  });
  client.SetConnReadCallback([](const anet::tcp::TcpConnectionPtr &conn, std::string_view data) {
//    if (data.length() == 0) return;
    std::cout << "Received: " << data << std::endl;
//    conn->Send(data);
//    conn->Send("Hello");
  });
  client.SetConnCloseCallback([](const anet::tcp::TcpConnection *conn) {
    std::cout << "closed" << std::endl;
  });
  client.AsyncConnect();
  io_context.run();
}
