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

TEST(TestToString, vanilla){
    {
        std::vector<char> buf;
        fix::to_string(0, buf);
        EXPECT_EQ(2, buf.size());
        EXPECT_EQ('0', buf[0]);
        EXPECT_EQ(1, buf[1]);
    }

    {
        std::vector<char> buf;
        fix::to_string(1, buf);
        EXPECT_EQ(2, buf.size());
        EXPECT_EQ('1', buf[0]);
        EXPECT_EQ(1, buf[1]);
    }
    {
        std::vector<char> buf;
        fix::to_string(-1, buf);
        EXPECT_EQ(3, buf.size());
        EXPECT_EQ('-', buf[0]);
        EXPECT_EQ('1', buf[1]);
        EXPECT_EQ(1, buf[2]);
    }
    {
        std::vector<char> buf;
        fix::to_string(2111222333, buf);
        EXPECT_EQ(11, buf.size());
        EXPECT_EQ('2', buf[0]);
        EXPECT_EQ('1', buf[1]);
        EXPECT_EQ('1', buf[2]);
        EXPECT_EQ('1', buf[3]);
        EXPECT_EQ('2', buf[4]);
        EXPECT_EQ('2', buf[5]);
        EXPECT_EQ('2', buf[6]);
        EXPECT_EQ('3', buf[7]);
        EXPECT_EQ('3', buf[8]);
        EXPECT_EQ('3', buf[9]);
        EXPECT_EQ(1, buf[10]);
    }
    {
        std::vector<char> buf;
        fix::to_string(-2111222333, buf);
        EXPECT_EQ(12, buf.size());
        EXPECT_EQ('-', buf[0]);
        EXPECT_EQ('2', buf[1]);
        EXPECT_EQ('1', buf[2]);
        EXPECT_EQ('1', buf[3]);
        EXPECT_EQ('1', buf[4]);
        EXPECT_EQ('2', buf[5]);
        EXPECT_EQ('2', buf[6]);
        EXPECT_EQ('2', buf[7]);
        EXPECT_EQ('3', buf[8]);
        EXPECT_EQ('3', buf[9]);
        EXPECT_EQ('3', buf[10]);
        EXPECT_EQ(1, buf[11]);
    }

}
