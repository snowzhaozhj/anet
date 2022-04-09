#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_CONTEXT_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_CONTEXT_HPP_

#include "anet/http/http_request_parser.hpp"
#include "anet/http/http_reply_parser.hpp"

namespace anet::http {

struct HttpServerContext {
  void Init() {
    parser.Bind(&request);
  }

  void Reset() {
    request.Reset();
    parser.Bind(&request);
  }

  HttpRequest request;
  HttpRequestParser parser;
};

struct HttpClientContext {
  void Init() {
    parser.Bind(&reply);
  }

  void Reset() {
    reply.Reset();
    parser.Bind(&reply);
  }

  HttpReply reply;
  HttpReplyParser parser;
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_CONTEXT_HPP_
