#ifndef ANET_INCLUDE_ANET_TCP_TCP_CONNECTION_SETTER_HPP_
#define ANET_INCLUDE_ANET_TCP_TCP_CONNECTION_SETTER_HPP_

#include "anet/tcp/tcp_connection.hpp"

namespace anet::tcp {

class TcpConnectionSetter {
 public:
  using NewConnCallback = std::function<void(const TcpConnectionPtr &)>;
  using ConnReadCallback = TcpConnection::ReadCallback;
  using ConnWriteCallback = TcpConnection::WriteCallback;
  using ConnCloseCallback = TcpConnection::CloseCallback;

  void SetNewConnCallback(const NewConnCallback &cb) { new_conn_callback_ = cb; }
  void SetConnReadCallback(const ConnReadCallback &cb) { conn_read_callback_ = cb; }
  void SetConnWriteCallback(const ConnWriteCallback &cb) { conn_write_callback_ = cb; }
  void SetConnCloseCallback(const ConnCloseCallback &cb) { conn_close_callback_ = cb; }

  void SetReadTimeout(util::Duration timeout) { read_timeout_ = timeout; }
  void SetWriteTimeout(util::Duration timeout) { write_timeout_ = timeout; }

  void InitConnection(const TcpConnectionPtr &conn) {
    conn->SetReadCallback(conn_read_callback_);
    conn->SetWriteCallback(conn_write_callback_);
    conn->SetCloseCallback(conn_close_callback_);
    conn->SetReadTimeout(read_timeout_);
    conn->SetWriteTimeout(write_timeout_);
  }

 protected:
  NewConnCallback new_conn_callback_;
  ConnReadCallback conn_read_callback_;
  ConnWriteCallback conn_write_callback_;
  ConnCloseCallback conn_close_callback_;

  util::Duration read_timeout_{0};
  util::Duration write_timeout_{0};
};

} // namespace anet::tcp

#endif //ANET_INCLUDE_ANET_TCP_TCP_CONNECTION_SETTER_HPP_
