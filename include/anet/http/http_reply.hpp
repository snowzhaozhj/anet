#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_REPLY_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_REPLY_HPP_

#include "anet/http/common.hpp"
#include "anet/util/string.hpp"

#include <map>

namespace anet::http {

class HttpReply {
 public:
  HttpReply() : version_(HttpVersion::Http11), status_code_(0) {}

  [[nodiscard]] HttpVersion GetVersion() const { return version_; }
  void SetVersion(HttpVersion version) { version_ = version; }

  [[nodiscard]] int GetStatusCode() const { return status_code_; }
  void SetStatusCode(int status_code) { status_code_ = status_code; }

  [[nodiscard]] std::string GetStatusMessage() const { return status_message_; }
  void SetStatusMessage(const std::string &status_message) { status_message_ = status_message; }

  /// @brief 同时设置status_code和status_message
  void SetStatus(HttpStatusCode status_code) {
    status_code_ = static_cast<int>(status_code);
    status_message_ = ToString(status_code);
  }

  [[nodiscard]] const std::string &GetHeader(const std::string &key,
                                             const std::string &default_value = "") const {
    auto pos = headers_.find(key);
    if (pos != headers_.end()) {
      return pos->second;
    } else {
      return default_value;
    }
  }
  void AddHeader(const std::string &key, const std::string &value) {
    headers_[key] = value;
  }

  void SetContentType(std::string_view content_type) {
    headers_[kContentTypeField] = content_type;
  }

  [[nodiscard]] const std::string &GetContent() const { return content_; }
  std::string &GetContent() { return content_; }
  void SetContent(const std::string &content) { content_ = content; }
  void SetContent(std::string &&content) { content_ = std::move(content); }

  void Reset() {
    version_ = HttpVersion::Http11;
    status_code_ = 0;
    status_message_.clear();
    headers_.clear();
    content_.clear();
  }

  std::string SerializedToString() const {
    AddConnectionField();
    AddContentLengthField();
    std::string str;
    str.append(ToString(version_));
    str.append(" ");
    str.append(std::to_string(status_code_));
    str.append(" ");
    str.append(status_message_);
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

  HttpVersion version_;
  int status_code_;
  std::string status_message_;
  mutable std::map<std::string, std::string, util::CaseInsensitiveLess> headers_;
  std::string content_;
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_REPLY_HPP_
