#ifndef ANET_INCLUDE_ANET_TCP_TCP_SERVER_HPP_
#define ANET_INCLUDE_ANET_TCP_TCP_SERVER_HPP_

#include "anet/tcp/tcp_connection_setter.hpp"
#include "anet/util/io_context_pool.hpp"

namespace anet::tcp {

class TcpServer : public TcpConnectionSetter {
 public:
  explicit TcpServer(int thread_num = 1)
      : io_context_pool_(thread_num),
        acceptor_(io_context_pool_.GetIOContext()) {
  }

  void Listen(std::string_view address, std::string_view port) {
    tcp::resolver resolver(io_context_pool_.GetIOContext());
    tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    DoAccept();
  }

  void Run() {
    io_context_pool_.Run();
  }

  void Stop() {
    io_context_pool_.Stop();
  }
 private:
  void DoAccept() {
    auto new_conn = std::make_shared<TcpConnection>(io_context_pool_.GetIOContext());
    InitConnection(new_conn);

    acceptor_.async_accept(new_conn->GetSocket(), [this, new_conn](std::error_code ec) {
      if (!ec) {
        if (new_conn_callback_) {
          new_conn_callback_(new_conn);
        }
        new_conn->Start();
      }
      DoAccept();
    });
  }

  util::IOContextPool io_context_pool_;
  tcp::acceptor acceptor_;
};

} // namespace anet::tcp

#endif //ANET_INCLUDE_ANET_TCP_TCP_SERVER_HPP_
