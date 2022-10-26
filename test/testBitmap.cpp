#include <gtest/gtest.h>
#include "../src/Utils.hpp"

using namespace fix;

TEST(TestBitmap, vanilla){
    BitMask val;
    EXPECT_EQ(64, val.size());
    EXPECT_FALSE(val.is_set(0));

    val.set(0);
    EXPECT_TRUE(val.is_set(0));

    val.unset(0);
    EXPECT_FALSE(val.is_set(0));

    val.set(0);
    val.set(63);
    EXPECT_TRUE(val.is_set(63));
    val.unset(63);
    EXPECT_FALSE(val.is_set(63));
}
