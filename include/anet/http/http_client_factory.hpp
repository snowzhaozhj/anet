#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_FACTORY_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_FACTORY_HPP_

#include "anet/http/http_client.hpp"

namespace anet::http {

class HttpClientFactory {
 public:
  using ResponseCallback = HttpClient::ResponseCallback;

  HttpClientFactory() {
    thread_ = std::make_shared<std::thread>([this] {
      io_context_.io_context_ptr->run();
    });
  }
  ~HttpClientFactory() {
    io_context_.guard.reset();
    thread_->join();
  }

  void AsyncGet(const std::string &host,
                const std::string &service,
                const std::string &path,
                const ResponseCallback &cb) {
    auto client = new HttpClient(*io_context_.io_context_ptr, host, service);
    client->SetCloseCallback([client](HttpClient &c) {
      delete client;
    });
    client->AsyncGet(path, cb);
  }
  void AsyncPost(const std::string &host,
                 const std::string &service,
                 const std::string &path,
                 const std::string &content,
                 const ResponseCallback &cb) {
    auto client = new HttpClient(*io_context_.io_context_ptr, host, service);
    client->SetCloseCallback([client](HttpClient &c) {
      delete client;
    });
    client->AsyncPost(path, content, cb);
  }
 private:

  util::GuardedIOContext io_context_;
  std::shared_ptr<std::thread> thread_;
};

} // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_CLIENT_FACTORY_HPP_
