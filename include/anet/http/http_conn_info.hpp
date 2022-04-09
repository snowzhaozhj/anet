#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_CONN_INFO_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_CONN_INFO_HPP_

#include "anet/tcp/tcp_connection.hpp"

namespace anet::http {

/// @brief 保存Http连接的一些信息，如本地endpoint, 对端endpoint
struct HttpConnInfo {
  tcp::Tcp::endpoint local_endpoint;
  tcp::Tcp::endpoint remote_endpoint;
};

inline HttpConnInfo MakeHttpConnInfo(const tcp::TcpConnectionPtr &conn) {
  return {
      .local_endpoint = conn->GetLocalEndpoint(),
      .remote_endpoint = conn->GetRemoteEndpoint(),
  };
}

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_CONN_INFO_HPP_
