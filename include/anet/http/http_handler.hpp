#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_HANDLER_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_HANDLER_HPP_

#include "anet/http/http_conn_info.hpp"
#include "anet/http/http_request.hpp"
#include "anet/http/http_reply.hpp"

namespace anet::http {

using HttpHandler = std::function<void(const HttpConnInfo &, const HttpRequest &, HttpReply &)>;

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_HANDLER_HPP_
