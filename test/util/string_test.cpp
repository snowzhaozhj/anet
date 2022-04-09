#include <anet/util/string.hpp>

#include "anet_test.hpp"

class StringUtilTest : public testing::Test {};

TEST_F(StringUtilTest, StringToInt) {
  {
    auto[success, val] = anet::util::StringToInt("9987");
    EXPECT_TRUE(success);
    EXPECT_EQ(val, 9987);
  }
  {
    auto[success, val] = anet::util::StringToInt("F9987");
    EXPECT_FALSE(success);
  }
  {
    auto[success, val] = anet::util::StringToInt("FFF9", 16);
    EXPECT_TRUE(success);
    EXPECT_EQ(val, 0xFFF9);
  }
}
