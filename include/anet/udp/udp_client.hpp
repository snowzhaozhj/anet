#ifndef ANET_INCLUDE_ANET_UDP_UDP_CLIENT_HPP_
#define ANET_INCLUDE_ANET_UDP_UDP_CLIENT_HPP_

#include "anet/udp/udp_connection_setter.hpp"

namespace anet::udp {

class UdpClient : public UdpConnectionSetter {
 public:
  using NewConnCallback = std::function<void(const UdpConnectionPtr &)>;

  UdpClient(asio::io_context &io_context) : io_context_(io_context) {}
  ~UdpClient() = default;

  void Connect(std::string_view host, std::string_view service, const NewConnCallback &cb) {
    Udp::resolver resolver(io_context_);
    Udp::endpoint endpoint = *resolver.resolve(host, service).begin();
    Udp::socket socket(io_context_);
    socket.open(endpoint.protocol());
    auto new_conn = std::make_shared<UdpConnection>(std::move(socket));
    InitConnection(new_conn);
    new_conn->SetSenderEndpoint(endpoint);
    if (cb) {
      cb(new_conn);
    }
  }
 private:
  asio::io_context &io_context_;
};

} // namespace anet::udp

#endif //ANET_INCLUDE_ANET_UDP_UDP_CLIENT_HPP_
