#include <anet/http/http_request.hpp>

#include "anet_test.hpp"

class HttpRequestTest : public testing::Test {
 public:
  anet::http::HttpRequest request_;
};

TEST_F(HttpRequestTest, Base) {
  request_.SetMethod(anet::http::HttpMethod::Get);
  EXPECT_EQ(request_.GetMethod(), anet::http::HttpMethod::Get);

  request_.SetVersion(anet::http::HttpVersion::Http11);
  EXPECT_EQ(request_.GetVersion(), anet::http::HttpVersion::Http11);

  request_.SetContent("hello, world!");
  EXPECT_EQ(request_.GetContent(), "hello, world!");
}

TEST_F(HttpRequestTest, Url) {
  request_.SetUrl("/index.html?a=b&c=d");
  EXPECT_EQ(request_.GetUrl(), "/index.html?a=b&c=d");
  EXPECT_EQ(request_.GetRouteUrl(), "/index.html");
  EXPECT_EQ(request_.GetRawParams(), "a=b&c=d");
}

TEST_F(HttpRequestTest, Header) {
  request_.AddHeader("Content-Length", "10");
  EXPECT_EQ(request_.GetHeader("Content-Length"), "10");
  EXPECT_EQ(request_.GetHeader("content-length"), "10");
  EXPECT_EQ(request_.GetHeader("Content-Type"), "");
  EXPECT_EQ(request_.GetHeader("Content-Type", "text/html"), "text/html");
  request_.AddHeader("Content-Type", "application/json");
  EXPECT_EQ(request_.GetHeader("Content-Type"), "application/json");
  EXPECT_EQ(request_.GetHeader("Content-Length"), "10");
  request_.AddHeader("content-length", "30");
  EXPECT_EQ(request_.GetHeader("Content-Length"), "30");
}
