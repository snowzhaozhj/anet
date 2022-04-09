#ifndef ANET_INCLUDE_ANET_TCP_TCP_CLIENT_HPP_
#define ANET_INCLUDE_ANET_TCP_TCP_CLIENT_HPP_

#include "anet/tcp/tcp_connection_setter.hpp"
#include "anet/util/io_context_pool.hpp"

#include <asio/connect.hpp>
#include <iostream>

namespace anet::tcp {

class TcpClient : public TcpConnectionSetter {
 public:
  static constexpr util::Duration kRetryInitDelay = std::chrono::milliseconds(500);

  explicit TcpClient(asio::io_context &io_context)
      : io_context_(io_context),
        resolver_(io_context),
        connection_(std::make_shared<TcpConnection>(io_context)),
        connect_timeout_(0),
        timeout_timer_(io_context),
        retry_(false),
        retry_delay_(kRetryInitDelay),
        retry_timer_(io_context) {}

  void SetConnectTimeout(util::Duration timeout) { connect_timeout_ = timeout; }
  void SetRetry(bool retry) { retry_ = retry; }

  void AsyncConnect(std::string_view host, std::string_view service) {
    resolver_.async_resolve(host, service, [this](std::error_code ec, const Tcp::resolver::results_type &endpoints) {
      AsyncConnect(endpoints);
    });
  }

  void AsyncConnect(const Tcp::resolver::results_type &endpoints) {
    InitConnection(connection_);
    endpoints_ = endpoints;
    if (connect_timeout_ != util::Duration(0)) {
      timeout_timer_.expires_after(connect_timeout_);
      timeout_timer_.async_wait([this](std::error_code) {
        connection_->DoClose();
        if (retry_) {
          retry_timer_.cancel();
        }
      });
    }

    DoConnect();
  }

  [[nodiscard]] const TcpConnectionPtr &GetConnection() const { return connection_; }

 private:
  void DoConnect() {
    asio::async_connect(connection_->GetSocket(),
                        endpoints_,
                        [this](std::error_code ec, auto &&) {
                          if (!ec) {
                            if (new_conn_callback_) {
                              new_conn_callback_(connection_);
                            }
                            connection_->Start();
                          } else {
                            if (retry_) {
                              retry_timer_.expires_after(retry_delay_);
                              retry_delay_ *= 2;
                              retry_timer_.async_wait([this](std::error_code) {
                                DoConnect();
                              });
                            }
                          }
                        });
  }

  asio::io_context &io_context_;
  Tcp::resolver resolver_;
  Tcp::resolver::results_type endpoints_;
  TcpConnectionPtr connection_;

  util::Duration connect_timeout_;
  asio::steady_timer timeout_timer_;

  bool retry_;
  util::Duration retry_delay_;
  asio::steady_timer retry_timer_;
};

} // namespace anet::tcp

#endif //ANET_INCLUDE_ANET_TCP_TCP_CLIENT_HPP_
