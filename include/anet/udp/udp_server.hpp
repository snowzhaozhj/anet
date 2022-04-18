#ifndef ANET_INCLUDE_ANET_UDP_UDP_SERVER_HPP_
#define ANET_INCLUDE_ANET_UDP_UDP_SERVER_HPP_

#include "anet/udp/udp_connection_setter.hpp"

namespace anet::udp {

class UdpServer : public UdpConnectionSetter {
 public:
  explicit UdpServer(asio::io_context &io_context) : io_context_(io_context) {}

  void Listen(std::string_view host, std::string_view service) {
    Udp::resolver resolver(io_context_);
    Udp::endpoint endpoint = *resolver.resolve(host, service).begin();
    Udp::socket socket(io_context_);
    socket.open(endpoint.protocol());
    socket.bind(endpoint);
    auto new_conn = std::make_shared<UdpConnection>(std::move(socket));
    InitConnection(new_conn);
    new_conn->DoReceive();
  }

 private:
  asio::io_context &io_context_;
};

} // namespace anet::udp

#endif //ANET_INCLUDE_ANET_UDP_UDP_SERVER_HPP_
