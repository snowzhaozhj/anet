#include <anet/http/http_client.hpp>
#include <iostream>

using anet::http::HttpClient;
using anet::http::HttpReply;
using anet::http::HttpRequest;

void ResponseCallback(HttpClient &client, const HttpReply &reply) {
  std::cout << reply.SerializedToString() << std::endl;
}

int main() {
  asio::io_context io_context;
  HttpClient client(io_context, "www.baidu.com", "80");
  client.SetResponseCallback(ResponseCallback);
  HttpRequest request;
  request.SetMethod(anet::http::HttpMethod::Get);
  request.SetUrl("/");
  client.AsyncRequest(request);
  io_context.run();
}
