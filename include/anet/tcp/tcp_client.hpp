#ifndef ANET_INCLUDE_ANET_TCP_TCP_CLIENT_HPP_
#define ANET_INCLUDE_ANET_TCP_TCP_CLIENT_HPP_

#include "anet/tcp/tcp_connection_setter.hpp"
#include "anet/util/io_context_pool.hpp"

#include <asio/connect.hpp>
#include <iostream>

namespace anet::tcp {

class TcpClient : public TcpConnectionSetter {
 public:
  using Duration = util::Duration;
  using ConnectTimeoutCallback = std::function<void(TcpClient &)>;

  explicit TcpClient(asio::io_context &io_context, std::string host,
                     std::string service)
      : io_context_(io_context),
        host_(std::move(host)),
        service_(std::move(service)),
        resolver_(io_context),
        connection_(),
        connect_timeout_(0),
        timeout_timer_(io_context),
        timeouted_(false) {}

  void SetConnectTimeout(Duration timeout) { connect_timeout_ = timeout; }
  void SetConnectTimeoutCallback(const ConnectTimeoutCallback &cb) {
    timeout_callback_ = cb;
  }

  [[nodiscard]] const std::string &GetHost() const { return host_; }
  [[nodiscard]] const std::string &GetService() const { return service_; }

  void StartConnect() {
    if (connect_timeout_ != Duration(0)) {
      timeout_timer_.expires_after(connect_timeout_);
      timeout_timer_.async_wait([this](std::error_code ec) {
        if (!ec) {
          timeouted_ = true;
        }
      });
    }
    AsyncConnect();
  }

  [[nodiscard]] TcpConnectionPtr GetConnection() const {
    return connection_.lock();
  }

 private:
  void AsyncConnect() {
    auto conn = std::make_shared<TcpConnection>(io_context_);
    connection_ = conn;
    InitConnection(conn);
    resolver_.async_resolve(host_, service_,
                            [this, conn](std::error_code ec, const Tcp::resolver::results_type &endpoints) {
                              DoConnect(endpoints);
                            });
  }

  void DoConnect(const Tcp::resolver::results_type &endpoints) {
    auto conn = connection_.lock();
    asio::async_connect(conn->GetSocket(), endpoints, [this, conn](std::error_code ec, auto &&) {
      if (!ec) {
        if (connect_timeout_ != Duration(0)) {
          timeout_timer_.cancel();
        }
        if (new_conn_callback_) {
          new_conn_callback_(conn);
        }
      } else {
        if (!timeouted_) {
          AsyncConnect();
        } else {
          timeout_callback_(*this);
        }
      }
    });
  }

  asio::io_context &io_context_;
  std::string host_;
  std::string service_;
  Tcp::resolver resolver_;
  std::weak_ptr<TcpConnection> connection_;

  Duration connect_timeout_;
  asio::steady_timer timeout_timer_;
  bool timeouted_;
  ConnectTimeoutCallback timeout_callback_;
};

} // namespace anet::tcp

#endif //ANET_INCLUDE_ANET_TCP_TCP_CLIENT_HPP_
