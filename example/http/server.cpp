#include <anet/http/http_server.hpp>
#include <iostream>

using anet::http::HttpConnInfo;
using anet::http::HttpRequest;
using anet::http::HttpReply;
using anet::http::HttpServer;

void HelloHandler(const HttpConnInfo &conn_info, const HttpRequest &request, HttpReply &reply) {
  reply.SetContent("Hello");
}

int main() {
  HttpServer server;
  server.Handle("/", HelloHandler);
  server.Listen("localhost", "9987");
  server.Run();
}
