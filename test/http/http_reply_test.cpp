#include <anet/http/http_reply.hpp>

#include "anet_test.hpp"

class HttpReplyTest : public testing::Test {
 public:
  anet::http::HttpReply reply_;
};

TEST_F(HttpReplyTest, Base) {
  reply_.SetVersion(anet::http::HttpVersion::Http11);
  EXPECT_EQ(reply_.GetVersion(), anet::http::HttpVersion::Http11);

  reply_.SetStatusCode(501);
  EXPECT_EQ(reply_.GetStatusCode(), 501);

  reply_.SetStatusMessage("Not Implemented");
  EXPECT_EQ(reply_.GetStatusMessage(), "Not Implemented");

  reply_.SetContent("hello, world!");
  EXPECT_EQ(reply_.GetContent(), "hello, world!");
}

TEST_F(HttpReplyTest, Header) {
  reply_.AddHeader("Content-Length", "10");
  EXPECT_EQ(reply_.GetHeader("Content-Length"), "10");
  EXPECT_EQ(reply_.GetHeader("content-length"), "10");
  EXPECT_EQ(reply_.GetHeader("Content-Type"), "");
  EXPECT_EQ(reply_.GetHeader("Content-Type", "text/html"), "text/html");
  reply_.AddHeader("Content-Type", "application/json");
  EXPECT_EQ(reply_.GetHeader("Content-Type"), "application/json");
  EXPECT_EQ(reply_.GetHeader("Content-Length"), "10");
  reply_.AddHeader("content-length", "30");
  EXPECT_EQ(reply_.GetHeader("Content-Length"), "30");
}
