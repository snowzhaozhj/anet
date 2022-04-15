#ifndef ANET_INCLUDE_ANET_TCP_TCP_SESSION_HPP_
#define ANET_INCLUDE_ANET_TCP_TCP_SESSION_HPP_

#include "anet/util/double_buffer.hpp"
#include "anet/util/chrono.hpp"

#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <asio/read.hpp>

namespace anet::tcp {

using Tcp = asio::ip::tcp;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  static constexpr std::size_t kReadBufferInitSize = 1024;

  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using ReadCallback = std::function<void(const TcpConnectionPtr &, std::string_view data)>;
  using WriteCallback = std::function<void(const TcpConnectionPtr &)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;

  explicit TcpConnection(asio::io_context &io_context)
      : socket_(io_context),
        closed_(false),
        read_buffer_(kReadBufferInitSize),
        read_timeout_(0),
        write_timeout_(0),
        timeout_timer_(io_context) {}

  ~TcpConnection() {
    DoClose();
  }

  Tcp::socket &GetSocket() { return socket_; }
  Tcp::endpoint GetLocalEndpoint() {
    return socket_.local_endpoint();
  }
  Tcp::endpoint GetRemoteEndpoint() {
    return socket_.remote_endpoint();
  }

  /// @brief ReadCallback将会在每次读出一段数据后调用, Callback中的string_view参数指向读出的数据, 读出的数据会在下一次读的时候被覆盖
  void SetReadCallback(const ReadCallback &cb) { read_callback_ = cb; }
  /// @brief WriteCallback将会在每次写完一段数据后被调用
  void SetWriteCallback(const WriteCallback &cb) { write_callback_ = cb; }
  /// @brief CloseCallback将会在主动调用DoClose或者在读数据和写数据失败的时候被调用
  void SetCloseCallback(const CloseCallback &cb) { close_callback_ = cb; }

  void SetReadTimeout(util::Duration timeout) { read_timeout_ = timeout; }
  void SetWriteTimeout(util::Duration timeout) { write_timeout_ = timeout; }

  void SetContext(const std::any &context) { context_ = context; }
  const std::any &GetContext() const { return context_; }
  std::any &GetContext() { return context_; }

  void Start() {
    DoRead();
  }

  /// @brief 进行异步读数据
  /// @note 会覆盖read_buffer中的数据
  void DoRead() {
    if (read_timeout_ != util::Duration(0)) {
      timeout_timer_.expires_after(read_timeout_);
      timeout_timer_.async_wait([this](std::error_code) { DoClose(); });
    }
    socket_.async_read_some(
        asio::buffer(read_buffer_),
        [this, self = shared_from_this()](std::error_code ec, std::size_t len) {
          if (ec) {
            DoClose();
          } else {
            if (read_timeout_ != util::Duration(0)) {
              timeout_timer_.cancel();
            }
            if (read_callback_) {
              read_callback_(self, std::string_view(read_buffer_.data(), len));
            }
            if (len == read_buffer_.size()) { // 读满了就进行2倍扩容
              read_buffer_.resize(2 * len);
            }
          }
        });
  }
  /// @brief 异步读取n个字符
  /// @note 会覆盖read_buffer中的数据
  void DoRead(std::size_t n) {
    if (read_buffer_.size() < n) {
      read_buffer_.resize(n);
    }
    if (read_timeout_ != util::Duration(0)) {
      timeout_timer_.expires_after(read_timeout_);
      timeout_timer_.async_wait([this](std::error_code) { DoClose(); });
    }
    asio::async_read(socket_,
                     asio::buffer(read_buffer_, n),
                     [this, n, self = shared_from_this()](std::error_code ec, std::size_t) {
                       if (ec) {
                         DoClose();
                       } else {
                         if (read_timeout_ != util::Duration(0)) {
                           timeout_timer_.cancel();
                         }
                         if (read_callback_) {
                           read_callback_(self, std::string_view(read_buffer_.data(), n));
                         }
                       }
                     });
  }

  /// @brief 关闭socket
  void DoClose() {
    if (closed_) return;
    std::error_code ec;
    socket_.close(ec);
    timeout_timer_.cancel();
    closed_ = true;
    if (close_callback_) {
      CloseCallback(shared_from_this());
    }
  }

  void DoShutdown(Tcp::socket::shutdown_type what) {
    std::error_code ec;
    socket_.shutdown(what, ec);
  }

  /// @brief 向对端发送数据
  /// @note 非线程安全
  void Send(std::string_view s) {
    Send(s.data(), s.size());
  }

  /// @brief 向对端发送数据
  /// @note 非线程安全
  void Send(const char *data, std::size_t len) {
    write_buffer_.GetInactiveBuffer().append(data, len);
    if (!IsWriting()) {
      DoWrite();
    }
  }

 private:
  /// @brief 异步写数据
  void DoWrite() {
    if (write_timeout_ != util::Duration(0)) {
      timeout_timer_.expires_after(write_timeout_);
      timeout_timer_.async_wait([this](std::error_code) { DoClose(); });
    }
    write_buffer_.SwitchBuffer();
    asio::async_write(socket_,
                      asio::buffer(write_buffer_.GetActiveBuffer()),
                      [this, self = shared_from_this()](std::error_code ec, std::size_t len) {
                        if (ec) {
                          DoClose();
                        } else {
                          if (write_timeout_ != util::Duration(0)) {
                            timeout_timer_.cancel();
                          }
                          // 已经写入完毕，可以把数据清空
                          write_buffer_.GetActiveBuffer().clear();
                          if (write_callback_) {
                            write_callback_(self);
                          }
                          if (!write_buffer_.GetInactiveBuffer().empty()) {
                            // 有新的可写数据，则添加一个新的写操作
                            DoWrite();
                          }
                        }
                      });
  }

  bool IsWriting() {
    return !write_buffer_.GetActiveBuffer().empty();
  }

  Tcp::socket socket_;
  bool closed_; ///< socket是否被关闭

  std::vector<char> read_buffer_;
  util::DoubleBuffer<std::string> write_buffer_;

  util::Duration read_timeout_;
  util::Duration write_timeout_;
  asio::steady_timer timeout_timer_;

  ReadCallback read_callback_;
  WriteCallback write_callback_;
  CloseCallback close_callback_;

  std::any context_;  ///< 用于传递上下文信息
};

using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;

} // namespace anet::tcp

#endif //ANET_INCLUDE_ANET_TCP_TCP_SESSION_HPP_
