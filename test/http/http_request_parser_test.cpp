#include <anet/http/http_request_parser.hpp>

#include "anet_test.hpp"

class HttpRequestParserTest : public testing::Test {
 public:
  void SetUp() override {
    parser_.Bind(&request_);
  }

  anet::http::HttpRequest request_;
  anet::http::HttpRequestParser parser_;
};

TEST_F(HttpRequestParserTest, WholeWithNoContentSuccess) {
  std::string req_str = "GET / HTTP/1.1\r\n"
                        "Host: snowzhao.tk\r\n"
                        "Connection: Close\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n";
  parser_.Parse(req_str);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetMethod(), anet::http::HttpMethod::Get);
  EXPECT_EQ(request_.GetUrl(), "/");
  EXPECT_EQ(request_.GetVersion(), anet::http::HttpVersion::Http11);
  EXPECT_EQ(request_.GetHeader("Host"), "snowzhao.tk");
  EXPECT_EQ(request_.GetHeader("Connection"), "Close");
  EXPECT_EQ(request_.GetHeader("Content-Length"), "0");
  EXPECT_EQ(request_.GetContent(), "");
}

TEST_F(HttpRequestParserTest, WholeWithContentSuccess) {
  std::string req_str = "POST /home HTTP/1.0\r\n"
                        "Content-Length: 20\r\n"
                        "\r\n"
                        "abcdefghij"
                        "abcdefghij";
  parser_.Parse(req_str);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetMethod(), anet::http::HttpMethod::Post);
  EXPECT_EQ(request_.GetUrl(), "/home");
  EXPECT_EQ(request_.GetVersion(), anet::http::HttpVersion::Http10);
  EXPECT_EQ(request_.GetHeader("Content-Length"), "20");
  EXPECT_EQ(request_.GetContent(), "abcdefghij"
                                   "abcdefghij");
}

TEST_F(HttpRequestParserTest, ChunkWithNoContentSuccess) {
  std::string part1 = "GET /hello HTTP/1.1\r\n"
                      "Content-Type: text/html\r\n"
                      "Transfer-Encoding: chunked\r\n"
                      "\r\n";
  std::string part2 = "0\r\n\r\n";
  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetMethod(), anet::http::HttpMethod::Get);
  EXPECT_EQ(request_.GetUrl(), "/hello");
  EXPECT_EQ(request_.GetVersion(), anet::http::HttpVersion::Http11);
  EXPECT_EQ(request_.GetHeader("Transfer-Encoding"), "chunked");
  EXPECT_EQ(request_.GetHeader("Content-Type"), "text/html");
  parser_.Parse(part2);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetContent(), "");
}

TEST_F(HttpRequestParserTest, ChunkWithContentSuccess) {
  std::string part1 = "GET /hello HTTP/1.1\r\n"
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
  EXPECT_EQ(request_.GetContent(), "abcde");
  parser_.Parse(part3);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetContent(), "abcdefghij");
  parser_.Parse(part4);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
}

TEST_F(HttpRequestParserTest, SeparateWithContentSuccess) {
  std::string part1 = "GET /";
  std::string part2 = " HTTP/1.1\r";
  std::string part3 = "\nHost:";
  std::string part4 = " snowzhao.tk\r\n"
                      "Connection: Close\r\n"
                      "Content-Length: 20\r\n";
  std::string part5 = "\r\n";
  std::string part6 = "abcdefghij";
  std::string part7 = "abcdefghij";

  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetMethod(), anet::http::HttpMethod::Get);

  parser_.Parse(part2);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetUrl(), "/");
  EXPECT_EQ(request_.GetVersion(), anet::http::HttpVersion::Http11);

  parser_.Parse(part3);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(parser_.GetNeedByteNum(), 0);

  parser_.Parse(part4);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetHeader("Host"), "snowzhao.tk");
  EXPECT_EQ(request_.GetHeader("Connection"), "Close");
  EXPECT_EQ(request_.GetHeader("Content-Length"), "20");

  parser_.Parse(part5);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(parser_.GetNeedByteNum(), 20);

  parser_.Parse(part6);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetContent(), "abcdefghij");
  EXPECT_EQ(parser_.GetNeedByteNum(), 10);

  parser_.Parse(part7);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_TRUE(parser_.ParseFinished());
  EXPECT_EQ(request_.GetContent(), "abcdefghij"
                                   "abcdefghij");
  EXPECT_EQ(parser_.GetNeedByteNum(), 0);
}

TEST_F(HttpRequestParserTest, MethodFailed) {
  std::string req_str = "GETA /home HTTP/1.1\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpRequestParserTest, UrlFailed) {
  std::string req_str = "GET  HTTP/1.1\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpRequestParserTest, VersionFailed) {
  std::string req_str = "GET / HTTP/1.5\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpRequestParserTest, HeaderFailed) {
  std::string req_str = "GET / HTTP/1.1\r\n"
                        ": Hello\r\n";
  parser_.Parse(req_str);
  EXPECT_TRUE(parser_.ParseFailed());
}

TEST_F(HttpRequestParserTest, ChunkMismatchFailed) {
  std::string part1 = "GET / HTTP/1.1\r\n"
                      "Transfer-Encoding: chunked\r\n"
                      "\r\n";
  std::string part2 = "5\r\nabcd\r\n";
  parser_.Parse(part1);
  EXPECT_FALSE(parser_.ParseFailed());
  EXPECT_FALSE(parser_.ParseFinished());
  parser_.Parse(part2);
  EXPECT_TRUE(parser_.ParseFailed());
}

