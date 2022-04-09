#ifndef ANET_INCLUDE_ANET_HTTP_HTTP_REQUEST_PARSER_HPP_
#define ANET_INCLUDE_ANET_HTTP_HTTP_REQUEST_PARSER_HPP_

#include "anet/http/http_request.hpp"

namespace anet::http {

class HttpRequestParser {
  enum class ParseStatus {
    ParseMethod,
    ParseUrl,
    ParseVersion,
    ParseCRLF1,     ///< 请求行之后的CRLF
    ParseHeaderKey,
    ParseSpaceBeforeValue,
    ParseHeaderValue,
    ParseCRLF2,     ///< 每个头部后面的CRLF
    ParseCRLF3,     ///< 标识头部结束的CRLF
    ParseChunkLength,
    ParseCRLF4,     ///< chunk长度字段后的CRLF
    ParseChunkData,
    ParseCRLF5,    ///< chunk数据字段后的CRLF
    ParseLengthedBody,
    ParseFailed,
    ParseFinished,
  };
 public:
  HttpRequestParser()
      : request_(nullptr),
        parse_status_(ParseStatus::ParseMethod),
        content_length_(0),
        chunk_length_(0) {}

  void Bind(HttpRequest *request) {
    request_ = request;
    parse_status_ = ParseStatus::ParseMethod;
    buffer1.clear();
    buffer2.clear();
    content_length_ = 0;
    chunk_length_ = 0;
  }

  bool ParseFinished() { return parse_status_ == ParseStatus::ParseFinished; }
  bool ParseFailed() { return parse_status_ == ParseStatus::ParseFailed; }

  /// @brief 当解析器正在解析一个包含Content-Length字段的请求的Body部分时，此函数会返回还需要多少byte; 其余情况直接返回0
  std::size_t GetNeedByteNum() {
    if (parse_status_ != ParseStatus::ParseLengthedBody) return 0;
    return content_length_ - request_->GetContent().size();
  }

  /// @note 确保Parse前已经调用Bind方法绑定到一个HttpRequest对象
  void Parse(std::string_view s) {
    Parse(s.data(), s.size());
  }
  /// @note 确保Parse前已经调用Bind方法绑定到一个HttpRequest对象
  void Parse(const char *data, std::size_t len) {
    for (int i = 0; i < len; ++i) {
      if (!Consume(data[i])) {
        parse_status_ = ParseStatus::ParseFailed;
        return;
      }
      if (ParseFinished()) return;
    }
  }
 private:
  /// @return true表示成功消费，false表示消费失败(出现解析错误)
  bool Consume(char c) {
    switch (parse_status_) {
      case ParseStatus::ParseMethod:
        if (c == kSpace) {
          parse_status_ = ParseStatus::ParseUrl;
          return FillMethod();
        }
        buffer1.push_back(c);
        return true;
      case ParseStatus::ParseUrl:
        if (c == kSpace) {
          parse_status_ = ParseStatus::ParseVersion;
          return FillUrl();
        }
        buffer1.push_back(c);
        return true;
      case ParseStatus::ParseVersion:
        if (c == kCR) {
          parse_status_ = ParseStatus::ParseCRLF1;
          return FillVersion();
        }
        buffer1.push_back(c);
        return true;
      case ParseStatus::ParseCRLF1:
        if (c != kLF) return false;
        parse_status_ = ParseStatus::ParseHeaderKey;
        return true;
      case ParseStatus::ParseHeaderKey:
        if (c == kCR) { // Header结束'\r\n\r\n'
          parse_status_ = ParseStatus::ParseCRLF3;
          return true;
        } else if (c == kColon) {
          parse_status_ = ParseStatus::ParseSpaceBeforeValue;
          return true;
        }
        buffer1.push_back(c);
        return true;
      case ParseStatus::ParseSpaceBeforeValue:
        if (c != kSpace) return false;
        parse_status_ = ParseStatus::ParseHeaderValue;
        return true;
      case ParseStatus::ParseHeaderValue:
        if (c == kCR) {
          parse_status_ = ParseStatus::ParseCRLF2;
          return FillHeader();
        }
        buffer2.push_back(c);
        return true;
      case ParseStatus::ParseCRLF2:
        if (c != kLF) return false;
        parse_status_ = ParseStatus::ParseHeaderKey;
        return true;
      case ParseStatus::ParseCRLF3: {
        if (c != kLF) return false;
        content_length_ = 0;
        std::string value = request_->GetHeader(kContentLengthField);
        if (value.empty()) {  // 没有Content-Length字段
          value = request_->GetHeader(kTransferEncodingField);
          if (value == kTransferEncodingChunkded) { // 按chunk模式传输
            parse_status_ = ParseStatus::ParseChunkLength;
            return true;
          }
          /// 其他情况视为Content-Length为0
          parse_status_ = ParseStatus::ParseFinished;
          return true;
        } else {
          if (!util::StringToInt(content_length_, value)) return false;
          if (content_length_ == 0) {
            parse_status_ = ParseStatus::ParseFinished;
          } else {
            parse_status_ = ParseStatus::ParseLengthedBody;
          }
        }
        return true;
      }
      case ParseStatus::ParseChunkLength:
        if (c == kCR) {
          parse_status_ = ParseStatus::ParseCRLF4;
          return FillChunkLength();
        }
        buffer1.push_back(c);
        return true;
      case ParseStatus::ParseCRLF4:
        if (c != kLF) return false;
        parse_status_ = ParseStatus::ParseChunkData;
        return true;
      case ParseStatus::ParseChunkData:
        if (c == kCR) {
          parse_status_ = ParseStatus::ParseCRLF5;
          return FillChunkData();
        }
        buffer1.push_back(c);
        return true;
      case ParseStatus::ParseCRLF5:
        if (c != kLF) return false;
        if (chunk_length_ == 0) { // chunk传输完毕
          parse_status_ = ParseStatus::ParseFinished;
          FillContentLength();
        } else {
          parse_status_ = ParseStatus::ParseChunkLength;
        }
        return true;
      case ParseStatus::ParseLengthedBody:
        request_->GetContent().push_back(c);
        if (request_->GetContent().size() == content_length_) {
          parse_status_ = ParseStatus::ParseFinished;
        }
        return true;
      case ParseStatus::ParseFailed:
        return false;
      case ParseStatus::ParseFinished:
        return true;  // 解析成功后，如果还有字符，直接忽略，最好确保不要出现这种情况
    }
    return false; // unreachable
  }

  bool FillMethod() {
    HttpMethod method = ToHttpMethod(buffer1);
    if (method == HttpMethod::Invalid) return false;
    request_->SetMethod(method);
    buffer1.clear();
    return true;
  }

  bool FillUrl() {
    if (buffer1.empty()) return false;  // url不能为空
    request_->SetUrl(buffer1);
    buffer1.clear();
    return true;
  }

  bool FillVersion() {
    HttpVersion version = ToHttpVersion(buffer1);
    if (version == HttpVersion::Invalid) return false;
    request_->SetVersion(version);
    buffer1.clear();
    return true;
  }

  bool FillHeader() {
    if (buffer1.empty()) return false;  // Key不能为空
    request_->AddHeader(buffer1, buffer2);
    buffer1.clear();
    buffer2.clear();
    return true;
  }

  bool FillChunkLength() {
    if (!util::StringToInt(chunk_length_, buffer1, 16)) return false;
    buffer1.clear();
    return true;
  }

  bool FillChunkData() {
    if (buffer1.size() != chunk_length_) return false;
    request_->GetContent().append(buffer1);
    content_length_ += chunk_length_;
    buffer1.clear();
    return true;
  }

  /// @brief chunk传输完毕后，填充HttpRequest里的Content-Length字段
  void FillContentLength() {
    request_->AddHeader(kContentLengthField, std::to_string(content_length_));
  }

  HttpRequest *request_;
  ParseStatus parse_status_;

  std::string buffer1;
  std::string buffer2;

  int content_length_;  ///< 有Content-Length字段时，存储其值；chunk传输时，用来统计Body总共的长度，并在传输后进行填充
  int chunk_length_;    ///< chunk传输时的长度
};

}  // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_HTTP_REQUEST_PARSER_HPP_
