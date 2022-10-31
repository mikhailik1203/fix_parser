#include <gtest/gtest.h>
#include "../src/FIXParser.hpp"
#include "../src/dictionary/FIX44Builder.hpp"
#include "FIXTestBuilder.hpp"

using namespace fix;

TEST(TestFIXParser, serialize_UT1){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXMessageMetadata meta_msg("UT1", dict_fixTest);
    fix::FIXMessage test_msg = meta_msg.create();
    EXPECT_EQ(FIXProtocol::FIXTest, test_msg.protocol());
    EXPECT_EQ(FIXMessageType::UserDefined, test_msg.message_type().type());

    {// fill tags
        test_msg.set_int(fixTest_gen::IntTag1, 111);
        test_msg.set_bool(fixTest_gen::BoolTag1, true);
        test_msg.set_double(fixTest_gen::DoubleTag1, {"123.456"});
        test_msg.set_string(fixTest_gen::StringTag1, {"Test string value"});
        test_msg.set_date(fixTest_gen::DateTag1, {"20221024"});
        test_msg.set_datetime(fixTest_gen::DatetimeTag1, {"20221024 12:45:56"});
        test_msg.set_int(fixTest_gen::RawLenTag1, 13);
        test_msg.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002raw data \001\002"});
    }

    std::vector<char> buffer;
    test_msg.serialize(buffer);

    auto res = FIXParser::serialize(test_msg, true, true);
    EXPECT_EQ(fix::FIXProtocol::FIXTest, res.protocol());
    EXPECT_EQ("UT1", res.type().type_value().to_string());
    EXPECT_EQ(128, res.length());
    EXPECT_EQ("8=FIX.TEST\x1" "9=104\x1" "35=UT1\x1" "1=111\x1" "4=Y\x1" "7=123.456\x1" "13=Test string value\x1" 
            "16=20221024\x1" "19=20221024 12:45:56\x1" "22=13\x1" "23=\x1\x2raw data \x1\x2\x1" "10=055\x1", 
                std::string_view(res.buffer(), res.length()));
}

TEST(TestFIXParser, parse_UT1){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    const char raw_data[] = "8=FIX.4.4\x1" "9=104\x1" "35=UT1\x1" "1=111\x1" "4=Y\x1" "7=123.456\x1" "13=Test string value\x1" 
                "16=20221024\x1" "19=20221024 12:45:56\x1" "22=13\x1" "23=\x1\x2raw data \x1\x2\x1" "10=000\x1";
    size_t raw_data_len = sizeof(raw_data)/sizeof(char);
    FIXParser parser(dict_fixTest);
    auto msg = parser.parse(raw_data, raw_data_len);

    EXPECT_EQ(FIXMessageType::UserDefined, msg.message_type().type());
    EXPECT_EQ("UT1", msg.message_type().type_value().to_string());
    EXPECT_EQ(111, msg.get_int(fixTest_gen::IntTag1));
    EXPECT_TRUE(msg.get_bool(fixTest_gen::BoolTag1));
    EXPECT_EQ("123.456", msg.get_double(fixTest_gen::DoubleTag1).to_string());
    EXPECT_EQ("Test string value", msg.get_string(fixTest_gen::StringTag1).to_string());
    EXPECT_EQ("20221024", msg.get_date(fixTest_gen::DateTag1).to_string());
    EXPECT_EQ("20221024 12:45:56", msg.get_datetime(fixTest_gen::DatetimeTag1).to_string());
    EXPECT_EQ(13, msg.get_int(fixTest_gen::RawLenTag1));
    EXPECT_EQ("\001\002raw data \001\002", msg.get_rawdata(fixTest_gen::RawDataTag1).to_string());
}