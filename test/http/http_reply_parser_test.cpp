#include <anet/http/http_reply_parser.hpp>

#include "anet_test.hpp"

class HttpReplyParserTest : public testing::Test {
 public:
  void SetUp() override {
    parser_.Bind(&reply_);
  }

  anet::http::HttpReply reply_;
  anet::http::HttpReplyParser parser_;
};

TEST_F(HttpReplyParserTest, WholeWithNoContentSuccess) {
  std::string req_str = "HTTP/1.1 200 OK\r\n"
                        "Server: anet\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n";
  parser_.Parse(req_str);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetVersion(), anet::http::HttpVersion::Http11);
  EXPECT_EQ(reply_.GetStatusCode(), 200);
  EXPECT_EQ(reply_.GetStatusMessage(), "OK");
  EXPECT_EQ(reply_.GetHeader("Server"), "anet");
  EXPECT_EQ(reply_.GetHeader("Content-Length"), "0");
  EXPECT_EQ(reply_.GetContent(), "");
}

TEST_F(HttpReplyParserTest, WholeWithContentSuccess) {
  std::string req_str = "HTTP/1.0 404 Not Found\r\n"
                        "Content-Length: 20\r\n"
                        "\r\n"
                        "abcdefghij"
                        "abcdefghij";
  parser_.Parse(req_str);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetVersion(), anet::http::HttpVersion::Http10);
  EXPECT_EQ(reply_.GetStatusCode(), 404);
  EXPECT_EQ(reply_.GetStatusMessage(), "Not Found");
  EXPECT_EQ(reply_.GetHeader("Content-Length"), "20");
  EXPECT_EQ(reply_.GetContent(), "abcdefghij"
                                 "abcdefghij");
}

TEST_F(HttpReplyParserTest, ChunkWithNoContentSuccess) {
  std::string part1 = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Transfer-Encoding: chunked\r\n"
                      "\r\n";
  std::string part2 = "0\r\n\r\n";
  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetVersion(), anet::http::HttpVersion::Http11);
  EXPECT_EQ(reply_.GetHeader("Transfer-Encoding"), "chunked");
  EXPECT_EQ(reply_.GetHeader("Content-Type"), "text/html");
  parser_.Parse(part2);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetContent(), "");
}

TEST_F(HttpReplyParserTest, ChunkWithContentSuccess) {
  std::string part1 = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Transfer-Encoding: chunked\r\n"
                      "\r\n";
  std::string part2 = "5\r\nabcde\r\n";
  std::string part3 = "5\r\nfghij\r\n";
  std::string part4 = "0\r\n\r\n";
  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  parser_.Parse(part2);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetContent(), "abcde");
  parser_.Parse(part3);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetContent(), "abcdefghij");
  parser_.Parse(part4);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
}

TEST_F(HttpReplyParserTest, SeparateWithContentSuccess) {
  std::string part1 = "HTTP/1.1 2";
  std::string part2 = "00 OK\r";
  std::string part3 = "\nServer:";
  std::string part4 = " anet\r\n"
                      "Connection: Close\r\n"
                      "Content-Length: 20\r\n";
  std::string part5 = "\r\n";
  std::string part6 = "abcdefghij";
  std::string part7 = "abcdefghij";

  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetVersion(), anet::http::HttpVersion::Http11);

  parser_.Parse(part2);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetStatusCode(), 200);
  EXPECT_EQ(reply_.GetStatusMessage(), "OK");

  parser_.Parse(part3);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(parser_.GetNeedByteNum(), 0);

  parser_.Parse(part4);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetHeader("Server"), "anet");
  EXPECT_EQ(reply_.GetHeader("Connection"), "Close");
  EXPECT_EQ(reply_.GetHeader("Content-Length"), "20");

  parser_.Parse(part5);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(parser_.GetNeedByteNum(), 20);

  parser_.Parse(part6);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetContent(), "abcdefghij");
  EXPECT_EQ(parser_.GetNeedByteNum(), 10);

  parser_.Parse(part7);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(reply_.GetContent(), "abcdefghij"
                                 "abcdefghij");
  EXPECT_EQ(parser_.GetNeedByteNum(), 0);
}

TEST_F(HttpReplyParserTest, VersionFailed) {
  std::string req_str = "HTTP/1.5 200 OK\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpReplyParserTest, StatusCodeFailed) {
  std::string req_str = "HTTP/1.1 X00 OK\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpReplyParserTest, StatusMessageFailed) {
  std::string req_str = "HTTP/1.1 200 \r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpReplyParserTest, HeaderFailed) {
  std::string req_str = "HTTP/1.1 200 OK\r\n"
                        ": Hello\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpReplyParserTest, ChunkMismatchFailed) {
  std::string part1 = "HTTP/1.1 200 OK\r\n"
                      "Transfer-Encoding: chunked\r\n"
                      "\r\n";
  std::string part2 = "5\r\nabcd\r\n";
  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  parser_.Parse(part2);
  EXPECT_TRUE(parser_.ParseFailed());
}

