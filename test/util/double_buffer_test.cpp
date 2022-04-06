#include <anet/util/double_buffer.hpp>

#include "anet_test.hpp"

class DoubleBufferTest : public testing::Test {};

TEST_F(DoubleBufferTest, Base) {
  anet::util::DoubleBuffer<std::string> double_buffer;
  double_buffer.GetActiveBuffer() = "hello";
  double_buffer.GetInactiveBuffer() = "world";
  EXPECT_EQ(double_buffer.GetActiveBuffer(), "hello");
  EXPECT_EQ(double_buffer.GetInactiveBuffer(), "world");
  double_buffer.SwitchBuffer();
  EXPECT_EQ(double_buffer.GetActiveBuffer(), "world");
  EXPECT_EQ(double_buffer.GetInactiveBuffer(), "hello");
}

