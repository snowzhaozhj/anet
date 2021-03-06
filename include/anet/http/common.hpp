#ifndef ANET_INCLUDE_ANET_HTTP_COMMON_HPP_
#define ANET_INCLUDE_ANET_HTTP_COMMON_HPP_

#include <algorithm>
#include <string>
#include <unordered_map>

namespace anet::http {

inline constexpr char kSpace = ' ';
inline constexpr char kCR = '\r';
inline constexpr char kLF = '\n';
inline constexpr char kColon = ':';
inline constexpr char kCRLF[] = "\r\n";

inline constexpr char kLocationField[] = "Location";
inline constexpr char kContentLengthField[] = "Content-Length";
inline constexpr char kContentTypeField[] = "Content-Type";
inline constexpr char kConnectionField[] = "Connection";
inline constexpr char kConnectionClose[] = "close";
inline constexpr char kConnectionKeepAlive[] = "keep-alive";
inline constexpr char kTransferEncodingField[] = "Transfer-Encoding";
inline constexpr char kTransferEncodingChunkded[] = "chunked";

inline constexpr char k404Response[] = "HTTP/1.1 404 ";

inline constexpr char kHttpService[] = "http";
inline constexpr char kIndexFile[] = "index.html";

enum class HttpVersion {
  Invalid,
  Http10,
  Http11,
};

enum class HttpMethod {
  Invalid,
  Get,
  Head,
  Post,
  Put,
  Delete,
  Connect,
  Options,
  Trace,
  Patch,
};

enum class HttpStatusCode {
  kUnknown = 0,
  k200Ok = 200,
  k201Created = 201,
  k202Accepted = 202,
  k204NoContent = 204,
  k300MultipleChoices = 300,
  k301MovedPermanently = 301,
  k302MovedTemporarily = 302,
  k304NotModified = 303,
  k400BadRequest = 400,
  k401Unauthorized = 401,
  k403Forbidden = 403,
  k404NotFound = 404,
  k405MethodNotAllowed = 405,
  k409Conflict = 409,
  k429TooManyRequests = 429,
  k499ClientClosedRequest = 499,
  k500InternalServerError = 500,
  k501NotImplemented = 501,
  k502BadGateway = 502,
  k503ServiceUnavailable = 503,
  k504GatewayTimeout = 504,
};

namespace detail {

inline const std::unordered_map<HttpVersion, std::string> http_version_to_string = {
    {HttpVersion::Invalid, "INVALID"},
    {HttpVersion::Http10, "HTTP/1.0"},
    {HttpVersion::Http11, "HTTP/1.1"},
};

inline const std::unordered_map<HttpMethod, std::string> http_method_to_string = {
    {HttpMethod::Invalid, "INVALID"},
    {HttpMethod::Get, "GET"},
    {HttpMethod::Head, "HEAD"},
    {HttpMethod::Post, "POST"},
    {HttpMethod::Put, "PUT"},
    {HttpMethod::Delete, "DELETE"},
    {HttpMethod::Connect, "CONNECT"},
    {HttpMethod::Options, "OPTIONS"},
    {HttpMethod::Trace, "TRACE"},
    {HttpMethod::Patch, "PATCH"},
};

inline const std::unordered_map<HttpStatusCode, std::string> http_status_code_to_message = {
    {HttpStatusCode::kUnknown, "Unknown"},
    {HttpStatusCode::k200Ok, "OK"},
    {HttpStatusCode::k201Created, "Created"},
    {HttpStatusCode::k202Accepted, "Accepted"},
    {HttpStatusCode::k204NoContent, "No Content"},
    {HttpStatusCode::k300MultipleChoices, "Multiple Choices"},
    {HttpStatusCode::k301MovedPermanently, "Moved Permanently"},
    {HttpStatusCode::k302MovedTemporarily, "Moved Temporarily"},
    {HttpStatusCode::k304NotModified, "Not Modified"},
    {HttpStatusCode::k400BadRequest, "Bad Request"},
    {HttpStatusCode::k401Unauthorized, "Unauthorized"},
    {HttpStatusCode::k403Forbidden, "Forbidden"},
    {HttpStatusCode::k404NotFound, "Not Found"},
    {HttpStatusCode::k405MethodNotAllowed, "Method Not Allowed"},
    {HttpStatusCode::k409Conflict, "Conflict"},
    {HttpStatusCode::k429TooManyRequests, "Too Many Requests"},
    {HttpStatusCode::k499ClientClosedRequest, "Client Closed Request"},
    {HttpStatusCode::k500InternalServerError, "Internal Server Error"},
    {HttpStatusCode::k501NotImplemented, "Not Implemented"},
    {HttpStatusCode::k502BadGateway, "Bad Gateway"},
    {HttpStatusCode::k503ServiceUnavailable, "Service Unavailable"},
    {HttpStatusCode::k504GatewayTimeout, "Gateway Timeout"},
};

} // namespace anet::http::detail

inline std::string ToString(HttpVersion http_version) {
  auto pos = detail::http_version_to_string.find(http_version);
  if (pos != detail::http_version_to_string.end()) {
    return pos->second;
  }
  return "";
}

inline std::string ToString(HttpMethod http_method) {
  auto pos = detail::http_method_to_string.find(http_method);
  if (pos != detail::http_method_to_string.end()) {
    return pos->second;
  }
  return "";
}

inline std::string ToString(HttpStatusCode status_code) {
  auto pos = detail::http_status_code_to_message.find(status_code);
  if (pos != detail::http_status_code_to_message.end()) {
    return pos->second;
  }
  return "";
}

inline HttpVersion ToHttpVersion(const std::string &http_version) {
  auto pos = std::find_if(detail::http_version_to_string.begin(),
                          detail::http_version_to_string.end(),
                          [&http_version](const std::pair<HttpVersion, std::string> &pair) {
                            return pair.second == http_version;
                          });
  if (pos != detail::http_version_to_string.end()) {
    return pos->first;
  }
  return HttpVersion::Invalid;
}

inline HttpMethod ToHttpMethod(const std::string &http_method) {
  auto pos = std::find_if(detail::http_method_to_string.begin(),
                          detail::http_method_to_string.end(),
                          [&http_method](const std::pair<HttpMethod, std::string> &pair) {
                            return pair.second == http_method;
                          });
  if (pos != detail::http_method_to_string.end()) {
    return pos->first;
  }
  return HttpMethod::Invalid;
}

} // namespace anet::http

#endif //ANET_INCLUDE_ANET_HTTP_COMMON_HPP_
