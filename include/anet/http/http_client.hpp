#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_HPP_

#include "anet/http/http_context.hpp"
#include "anet/tcp/tcp_client.hpp"

namespace anet::http {

class HttpClient {
 public:
  using ResponseCallback = std::function<void(HttpClient &, const HttpReply &)>;
  using CloseCallback = std::function<void(HttpClient &)>;

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
    tcp_client_.SetConnCloseCallback([this](tcp::TcpConnection *conn, const tcp::Tcp::endpoint &remote_endpoint) {
      HandleConnClose(conn, remote_endpoint);
    });
    tcp_client_.SetConnErrorCallback([this](const tcp::TcpConnectionPtr &conn, std::error_code ec) {
      HandleConnError(conn, ec);
    });
  }

  void SetCloseCallback(const CloseCallback &cb) { close_callback_ = cb; }

  void AsyncGet(const std::string &url, const ResponseCallback &cb) {
    HttpRequest request;
    request.SetMethod(HttpMethod::Get);
    request.SetUrl(url);
    AsyncRequest(request, cb);
  }

  void AsyncPost(const std::string &url, const std::string &content, const ResponseCallback &cb) {
    HttpRequest request;
    request.SetMethod(HttpMethod::Post);
    request.SetUrl(url);
    request.SetContent(content);
    AsyncRequest(request, cb);
  }

  /// @note 只能同时调用一次，后续还要发送请求的话，请在callback里进行再次调用该函数
  void AsyncRequest(const HttpRequest &request, const ResponseCallback &cb) {
    response_callback_ = cb;
    request.AddHeader("Host", tcp_client_.GetHost());
    auto content = request.SerializedToString();
    if (connected_) {
      tcp_client_.GetConnection()->Send(std::move(content));
    } else {
      tcp_client_.SetNewConnCallback([this, c = std::move(content)](const tcp::TcpConnectionPtr &conn) mutable {
        HandleNewConn(conn);
        conn->Send(std::move(c));
      });
      tcp_client_.StartConnect();
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
    conn->DoRead();
  }
  void HandleConnClose(tcp::TcpConnection *conn, const tcp::Tcp::endpoint &remote_endpoint) {
    connected_ = false;
    if (close_callback_) {
      close_callback_(*this);
    }
  }
  void HandleConnError(const tcp::TcpConnectionPtr &conn, std::error_code ec) {
    // do nothing
  }

  tcp::TcpClient tcp_client_;
  bool connected_;

  ResponseCallback response_callback_;
  CloseCallback close_callback_;
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_HPP_
