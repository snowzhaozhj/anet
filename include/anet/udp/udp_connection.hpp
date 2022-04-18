#ifndef ANET_INCLUDE_ANET_UDP_UDP_CONNECTION_HPP_
#define ANET_INCLUDE_ANET_UDP_UDP_CONNECTION_HPP_

#include "anet/util/double_buffer.hpp"

#include <asio/ip/udp.hpp>

namespace anet::udp {

using Udp = asio::ip::udp;

class UdpConnection : public std::enable_shared_from_this<UdpConnection> {
 public:
  static constexpr std::size_t kReadBufferInitSize = 1024;

  using UdpConnectionPtr = std::shared_ptr<UdpConnection>;
  using ReadCallback = std::function<void(const UdpConnectionPtr &, std::string_view)>;
  using WriteCallback = std::function<void(const UdpConnectionPtr &)>;
  using CloseCallback = std::function<void(const UdpConnection *)>;

  explicit UdpConnection(Udp::socket socket)
      : socket_(std::move(socket)),
        closed_(false),
        read_buffer_(kReadBufferInitSize) {}

  void SetReadCallback(const ReadCallback &cb) { read_callback_ = cb; }
  void SetWriteCallback(const WriteCallback &cb) { write_callback_ = cb; }
  void SetCloseCallback(const CloseCallback &cb) { close_callback_ = cb; }

  const Udp::endpoint &GetSenderEndpoint() const { return sender_endpoint_; }
  void SetSenderEndpoint(const Udp::endpoint &endpoint) { sender_endpoint_ = endpoint; }

  void DoReceive() {
    socket_.async_receive_from(asio::buffer(read_buffer_),
                               sender_endpoint_,
                               [this, self = shared_from_this()](std::error_code ec, std::size_t len) {
                                 if (ec) {
                                   DoClose();
                                 } else {
                                   if (read_callback_) {
                                     read_callback_(shared_from_this(),
                                                    std::string_view(read_buffer_.data(), len));
                                   }
                                   if (len == read_buffer_.size()) {
                                     read_buffer_.resize(len * 2);
                                   }
                                 }
                               });
  }

  void Send(std::string_view data) {
    Send(data.data(), data.size());
  }
  void Send(const char *data, std::size_t len) {
    write_buffer_.GetInactiveBuffer().append(data, len);
    if (!IsWriting()) {
      DoSendTo();
    }
  }

  void Shutdown(Udp::socket::shutdown_type what) {
    std::error_code ec;
    socket_.shutdown(what, ec);
  }
  void DoClose() {
    if (closed_) return;
    std::error_code ec;
    socket_.close(ec);
    closed_ = true;
    if (close_callback_) {
      close_callback_(this);
    }
  }

 private:
  void DoSendTo() {
    write_buffer_.SwitchBuffer();
    socket_.async_send_to(asio::buffer(write_buffer_.GetActiveBuffer()),
                          sender_endpoint_,
                          [this, self = shared_from_this()](std::error_code ec, std::size_t len) {
                            if (ec) {
                              DoClose();
                            } else {
                              write_buffer_.GetActiveBuffer().clear();
                              if (write_callback_) {
                                write_callback_(shared_from_this());
                              }
                              if (!write_buffer_.GetInactiveBuffer().empty()) {
                                DoSendTo();
                              }
                            }
                          });
  }
  bool IsWriting() {
    return !write_buffer_.GetActiveBuffer().empty();
  }

  Udp::socket socket_;
  bool closed_;
  Udp::endpoint sender_endpoint_;
  std::vector<char> read_buffer_;
  util::DoubleBuffer<std::string> write_buffer_;

  ReadCallback read_callback_;
  WriteCallback write_callback_;
  CloseCallback close_callback_;
};

using UdpConnectionPtr = UdpConnection::UdpConnectionPtr;

} // namespace anet::udp

#endif //ANET_INCLUDE_ANET_UDP_UDP_CONNECTION_HPP_
