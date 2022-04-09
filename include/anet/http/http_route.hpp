#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_ROUTE_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_ROUTE_HPP_

#include "anet/http/http_handler.hpp"

namespace anet::http {

// 非常简陋的实现

class HttpRoute {
 public:
  HttpRoute() = default;

  void RegisterHandler(const std::string &path, const HttpHandler &handle_function) {
    path_to_handler_[path] = handle_function;
  }

  HttpHandler *GetHandler(const std::string &path) {
    auto pos = path_to_handler_.find(path);
    if (pos != path_to_handler_.end()) {
      return &(pos->second);
    }
    for (auto &[p, h] : path_to_handler_) {
      if (util::HasPrefix(path, p) ||
          util::HasPrefix(path + '/', p)) {
        return &h;
      }
    }
    return nullptr;
  }

 private:
  std::unordered_map<std::string, HttpHandler> path_to_handler_;
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_ROUTE_HPP_
