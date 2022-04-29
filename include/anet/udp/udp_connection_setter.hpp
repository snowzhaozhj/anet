#ifndef ANET_INCLUDE_ANET_UDP_UDP_CONNECTION_SETTER_HPP_
#define ANET_INCLUDE_ANET_UDP_UDP_CONNECTION_SETTER_HPP_

#include "anet/udp/udp_connection.hpp"

namespace anet::udp {

class UdpConnectionSetter {
 public:
  using ConnReadCallback = UdpConnection::ReadCallback;
  using ConnWriteCallback = UdpConnection::WriteCallback;
  using ConnCloseCallback = UdpConnection::CloseCallback;
  using ConnErrorCallback = UdpConnection::ErrorCallback;

  void SetConnReadCallback(const ConnReadCallback &cb) { conn_read_callback_ = cb; }
  void SetConnWriteCallback(const ConnWriteCallback &cb) { conn_write_callback_ = cb; }
  void SetConnCloseCallback(const ConnCloseCallback &cb) { conn_close_callback_ = cb; }

 protected:
  void InitConnection(const UdpConnectionPtr &conn) {
    conn->SetReadCallback(conn_read_callback_);
    conn->SetWriteCallback(conn_write_callback_);
    conn->SetCloseCallback(conn_close_callback_);
    conn->SetErrorCallback(conn_error_callback_);
  }

  ConnReadCallback conn_read_callback_;
  ConnWriteCallback conn_write_callback_;
  ConnCloseCallback conn_close_callback_;
  ConnErrorCallback conn_error_callback_;
};

} // namespace anet::udp

#endif //ANET_INCLUDE_ANET_UDP_UDP_CONNECTION_SETTER_HPP_
