#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_SERVER_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_SERVER_HPP_

#include "anet/http/http_context.hpp"
#include "anet/http/http_route.hpp"
#include "anet/tcp/tcp_server.hpp"

namespace anet::http {

class HttpServer {
 public:
  explicit HttpServer(int thread_num = 1) : tcp_server_(thread_num) {
    tcp_server_.SetNewConnCallback([this](const tcp::TcpConnectionPtr &conn) {
      HandleNewConn(conn);
    });
    tcp_server_.SetConnReadCallback([this](const tcp::TcpConnectionPtr &conn, std::string_view data) {
      HandleConnRead(conn, data);
    });
    tcp_server_.SetConnWriteCallback([this](const tcp::TcpConnectionPtr &conn) {
      HandleConnWrite(conn);
    });
    tcp_server_.SetConnCloseCallback([this](tcp::TcpConnection *conn, const tcp::Tcp::endpoint &remote_endpoint) {
      HandleConnClose(conn, remote_endpoint);
    });
  }

  void Listen(std::string_view address, std::string_view port) {
    tcp_server_.Listen(address, port);
  }

  void SetReadTimeout(util::Duration timeout) { tcp_server_.SetReadTimeout(timeout); }
  void SetWriteTimeout(util::Duration timeout) { tcp_server_.SetWriteTimeout(timeout); }

  void Handle(const std::string &path, const HttpHandler &handler) {
    route_.RegisterHandler(path, handler);
  }

  void Run() {
    tcp_server_.Run();
  }

  void Stop() {
    tcp_server_.Stop();
  }
 private:
  void HandleNewConn(const tcp::TcpConnectionPtr &conn) {
    conn->SetContext(std::make_any<HttpServerContext>());
    auto &context = std::any_cast<HttpServerContext &>(conn->GetContext());
    context.Init();
  }
  void HandleConnRead(const tcp::TcpConnectionPtr &conn, std::string_view data) {
    auto &context = std::any_cast<HttpServerContext &>(conn->GetContext());
    context.parser.Parse(data);
    if (context.parser.ParseFailed()) {
      conn->Send(k404Response);
      conn->DoClose();
      return;
    }
    if (context.parser.ParseFinished()) {
      HttpReply reply;
      auto handler = route_.GetHandler(context.request.GetRouteUrl());
      if (handler) {
        reply.SetStatus(HttpStatusCode::k200Ok);
        (*handler)(MakeHttpConnInfo(conn), context.request, reply);
      } else {
        reply.SetStatus(HttpStatusCode::k404NotFound);
      }
      conn->Send(reply.SerializedToString());
      if (context.request.GetHeader(kConnectionField) == kConnectionKeepAlive) {
        context.Reset();
        conn->DoRead();
      } else {
        conn->DoClose();
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
  void HandleConnClose(tcp::TcpConnection *conn, const tcp::Tcp::endpoint &remote_endpoint) {
    // do nothing
  }

  tcp::TcpServer tcp_server_;
  HttpRoute route_;
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_SERVER_HPP_
