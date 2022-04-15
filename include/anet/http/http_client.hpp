#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_HPP_

#include "anet/http/http_context.hpp"
#include "anet/tcp/tcp_client.hpp"

namespace anet::http {

class HttpClient {
 public:
  using ResponseCallback = std::function<void(HttpClient &, const HttpReply &)>;

  explicit HttpClient(asio::io_context &io_context, std::string host, std::string service)
      : tcp_client_(io_context, std::move(host), std::move(service)),
        connected_(false) {
    tcp_client_.SetNewConnCallback([this](const tcp::TcpConnectionPtr &conn) {
      HandleNewConn(conn);
    });
    tcp_client_.SetConnReadCallback([this](const tcp::TcpConnectionPtr &conn, std::string_view data) {
      HandleConnRead(conn, data);
    });
    tcp_client_.SetConnWriteCallback([this](const tcp::TcpConnectionPtr &conn) {
      HandleConnWrite(conn);
    });
    tcp_client_.SetConnCloseCallback([this](const tcp::TcpConnectionPtr &conn) {
      HandleConnClose(conn);
    });
  }

  void SetResponseCallback(const ResponseCallback &cb) {
    response_callback_ = cb;
  }

  /// @note 在调用此方法前，请先调用Bind函数绑定好host和service
  void AsyncRequest(const HttpRequest &request) {
    request.AddHeader("Host", tcp_client_.GetHost());
    auto content = request.SerializedToString();
    if (connected_) {
      tcp_client_.GetConnection()->Send(content);
    } else {
      tcp_client_.SetNewConnCallback([this, c = std::move(content)](const tcp::TcpConnectionPtr &conn) {
        HandleNewConn(conn);
        conn->Send(c);
      });
      tcp_client_.AsyncConnect();
    }
  }

  void SetConnectTimeout(util::Duration timeout) { tcp_client_.SetConnectTimeout(timeout); }
  void SetReadTimeout(util::Duration timeout) { tcp_client_.SetReadTimeout(timeout); }
  void SetWriteTimeout(util::Duration timeout) { tcp_client_.SetWriteTimeout(timeout); }

 private:
  void HandleNewConn(const tcp::TcpConnectionPtr &conn) {
    connected_ = true;
    conn->SetContext(std::make_any<HttpClientContext>());
    auto &context = std::any_cast<HttpClientContext &>(conn->GetContext());
    context.Init();
  }
  void HandleConnRead(const tcp::TcpConnectionPtr &conn, std::string_view data) {
    auto &context = std::any_cast<HttpClientContext &>(conn->GetContext());
    context.parser.Parse(data);
    if (context.parser.ParseFailed()) {
      return;
    }
    if (context.parser.ParseFinished()) {
      if (response_callback_) {
        response_callback_(*this, context.reply);
      }
      if (context.reply.GetHeader(kConnectionField) == kConnectionClose) {
        conn->DoClose();
      } else {
        context.Reset();
      }
    } else {  // 还未解析完成
      std::size_t n = context.parser.GetNeedByteNum();
      if (n) {  // 可以得知要成功解析当前Request还需要n个字节
        conn->DoRead(n);
      } else {  // 不知道还要读取多少字节
        conn->DoRead();
      }
    }
  }
  void HandleConnWrite(const tcp::TcpConnectionPtr &conn) {
    // do nothing
  }
  void HandleConnClose(const tcp::TcpConnectionPtr &conn) {
    connected_ = false;
  }

  tcp::TcpClient tcp_client_;
  bool connected_;

  ResponseCallback response_callback_;
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_HPP_
