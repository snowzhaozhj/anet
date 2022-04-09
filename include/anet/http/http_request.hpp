#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_REQUEST_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_REQUEST_HPP_

#include "anet/http/common.hpp"
#include "anet/util/string.hpp"

#include <map>

namespace anet::http {

class HttpRequest {
 public:
  HttpRequest() : method_(HttpMethod::Get), version_(HttpVersion::Http11) {}

  [[nodiscard]] HttpMethod GetMethod() const { return method_; }
  void SetMethod(HttpMethod method) { method_ = method; }

  [[nodiscard]] const std::string &GetUrl() const { return url_; }
  void SetUrl(const std::string &url) { url_ = url; }

  [[nodiscard]] std::string GetRouteUrl() const {
    auto pos = url_.find('?');
    return url_.substr(0, pos);
  }

  [[nodiscard]] std::string GetRawParams() const {
    auto pos = url_.find('?');
    return pos == std::string::npos ? "" : url_.substr(pos + 1);
  }

  [[nodiscard]] HttpVersion GetVersion() const { return version_; }
  void SetVersion(HttpVersion version) { version_ = version; }

  [[nodiscard]] const std::string &GetHeader(const std::string &key,
                                             const std::string &default_value = "") const {
    auto pos = headers_.find(key);
    if (pos != headers_.end()) {
      return pos->second;
    } else {
      return default_value;
    }
  }
  void AddHeader(const std::string &key, const std::string &value) const {
    headers_[key] = value;
  }

  [[nodiscard]] const std::string &GetContent() const { return content_; }
  std::string &GetContent() { return content_; }
  void SetContent(const std::string &content) { content_ = content; }
  void SetContent(std::string &&content) { content_ = std::move(content); }

  void Reset() {
    method_ = HttpMethod::Get;
    url_.clear();
    version_ = HttpVersion::Http11;
    headers_.clear();
    content_.clear();
  }

  std::string SerializedToString() const {
    AddConnectionField();
    AddContentLengthField();
    std::string str;
    str.append(ToString(method_));
    str.append(" ");
    str.append(url_);
    str.append(" ");
    str.append(ToString(version_));
    str.append(kCRLF);
    for (auto &[key, value] : headers_) {
      str.append(key);
      str.append(": ");
      str.append(value);
      str.append(kCRLF);
    }
    str.append(kCRLF);
    str.append(content_);
    return str;
  }

 private:
  void AddContentLengthField() const {
    if (headers_[kContentLengthField].empty()) {
      headers_[kContentLengthField] = std::to_string(content_.size());
    }
  }

  void AddConnectionField() const {
    if (headers_[kConnectionField].empty()) {
      headers_[kConnectionField] = kConnectionKeepAlive;
    }
  }

  HttpMethod method_;
  std::string url_;
  HttpVersion version_;
  mutable std::map<std::string, std::string, util::CaseInsensitiveLess> headers_;
  std::string content_;
};

} // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_REQUEST_HPP_
