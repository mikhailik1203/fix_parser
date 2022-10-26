#include <gtest/gtest.h>
#include "../src/FIXParserDefines.hpp"

using namespace fix;

TEST(TestFIXMsgBuffer, simple_create){
    fix::FIXMsgBuffer test_val;
    EXPECT_EQ(fix::FIXProtocol::invalid, test_val.protocol());
    EXPECT_EQ(0, test_val.length());
    EXPECT_EQ(FIXMessageType::invalid, test_val.type().type());
    EXPECT_EQ(nullptr, test_val.buffer());
}

TEST(TestFIXMsgBuffer, create_from_message){
    const char test_msg[] = "8=FIX.4.4\0019=7\00135=UAA\00110=111\001";
    const uint32_t msg_length = sizeof(test_msg)/sizeof(test_msg[0]);
    fix::FIXMsgBuffer test_val = fix::FIXMsgBuffer::create(&test_msg[0], msg_length);
    EXPECT_EQ(fix::FIXProtocol::FIX44, test_val.protocol());
    EXPECT_EQ(0, test_val.length()); //MsgType and value is skiped
    const auto &res_type = test_val.type();
    EXPECT_EQ(FIXMessageType::UserDefined, res_type.type());
    EXPECT_EQ(std::string_view("UAA", 3), res_type.type_value().value());
    EXPECT_NE(nullptr, test_val.buffer());
}

TEST(TestFIXMsgBuffer, create_when_msg_tail_missed){
    const char test_msg[] = "8=FIX.4.4\0019=5\00135=D\00110=111\001";
    const uint32_t msg_length = sizeof(test_msg)/sizeof(test_msg[0]);
    fix::FIXMsgBuffer test_val = fix::FIXMsgBuffer::create(&test_msg[0], msg_length - 1); 
    EXPECT_EQ(fix::FIXProtocol::invalid, test_val.protocol());
    EXPECT_EQ(0, test_val.length());
    EXPECT_EQ(FIXMessageType::invalid, test_val.type().type());
    EXPECT_EQ(nullptr, test_val.buffer());
}