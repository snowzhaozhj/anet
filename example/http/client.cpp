#include <anet/http/http_client_factory.hpp>
#include <iostream>

using anet::http::HttpClient;
using anet::http::HttpClientFactory;
using anet::http::HttpReply;
using anet::http::HttpRequest;

std::atomic<int> i = 0;

void ResponseCallback(HttpClient &client, const HttpReply &reply) {
  ++i;
  std::cout << i << ": reponse cb called" << std::endl;
//  std::cout << reply.SerializedToString() << std::endl;
}

HttpClientFactory factory;

void DoRequest() {
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.tencent.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.tencent.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
  factory.AsyncGet("www.google.com", "80", "/", ResponseCallback);
}

int main() {
  DoRequest();
//  asio::io_context io_context;
//  HttpClient client(io_context, "www.baidu.com", "80");
//  client.SetCloseCallback([](HttpClient &c) {
//    std::cout << "closed";
//  });
//  HttpRequest request;
//  request.SetMethod(anet::http::HttpMethod::Get);
//  request.SetUrl("/");
//  client.AsyncRequest(request, ResponseCallback);
//  io_context.run();
}
