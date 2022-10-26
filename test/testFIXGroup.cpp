#include <gtest/gtest.h>
#include "../src/FIXGroup.hpp"
#include "../src/dictionary/FIX44Builder.hpp"
#include "FIXTestBuilder.hpp"

using namespace fix;

TEST(TestFIXGroup, create_vanilla){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata meta_grp(fix44_gen::NoMdEntryTypes, dict_fix44);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_grp.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());
}

TEST(TestFIXGroup, serialize_empty){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata meta_grp(fix44_gen::NoMdEntryTypes, dict_fix44);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_grp.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    std::vector<char> buffer;
    test_grp.serialize(buffer);
    EXPECT_EQ(0, buffer.size());
}

TEST(TestFIXGroup, serialize_single_entry){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata meta_grp(fix44_gen::NoMdEntryTypes, dict_fix44);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_grp.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    auto &grp_entry = test_grp.add_entry();
    EXPECT_TRUE(grp_entry.set_int(fix44_gen::MDEntryType, 7));
    EXPECT_EQ(1, test_grp.size());

    std::vector<char> buffer;
    test_grp.serialize(buffer);
    EXPECT_EQ(12, buffer.size());
    EXPECT_EQ("267=1\001269=7\001", std::string(buffer.begin(), buffer.end()));
}

TEST(TestFIXGroup, serialize_multiple_entries){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata meta_grp(fix44_gen::NoMdEntryTypes, dict_fix44);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_grp.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    {
        auto &grp_entry = test_grp.add_entry();
        EXPECT_TRUE(grp_entry.set_int(fix44_gen::MDEntryType, 17));
    }
    {
        auto &grp_entry = test_grp.add_entry();
        EXPECT_TRUE(grp_entry.set_int(fix44_gen::MDEntryType, 27));
    }
    {
        auto &grp_entry = test_grp.add_entry();
        EXPECT_TRUE(grp_entry.set_int(fix44_gen::MDEntryType, 37));
    }
    {
        auto &grp_entry = test_grp.add_entry();
        EXPECT_TRUE(grp_entry.set_int(fix44_gen::MDEntryType, 47));
    }
    {
        auto &grp_entry = test_grp.add_entry();
        EXPECT_TRUE(grp_entry.set_int(fix44_gen::MDEntryType, 57));
    }

    EXPECT_EQ(5, test_grp.size());

    std::vector<char> buffer;
    test_grp.serialize(buffer);
    EXPECT_EQ(41, buffer.size());
    EXPECT_EQ("267=5\001269=17\001269=27\001269=37\001269=47\001269=57\001", std::string(buffer.begin(), buffer.end()));
}

TEST(TestFIXGroup, parse_single_entry){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata meta_grp(fix44_gen::NoMdEntryTypes, dict_fix44);
    MsgReceived msg_data{"267=1\001269=7\00110=111\001", 0, "", 0};

    fix::FIXGroup test_grp = meta_grp.parse(msg_data);
    EXPECT_TRUE(msg_data.error_.empty());
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_grp.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_grp.start_entry_tag());
    EXPECT_EQ(1, test_grp.size());
    auto &grp_entry = test_grp.entry(0);
    EXPECT_EQ(7, grp_entry.get_int(fix44_gen::MDEntryType));
}

TEST(TestFIXGroup, parse_multiple_entries){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata meta_grp(fix44_gen::NoMdEntryTypes, dict_fix44);
    MsgReceived msg_data{"267=5\001269=17\001269=27\001269=37\001269=47\001269=57\00110=111\001", 0, "", 0};

    fix::FIXGroup test_grp = meta_grp.parse(msg_data);
    EXPECT_TRUE(msg_data.error_.empty());
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_grp.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_grp.start_entry_tag());
    EXPECT_EQ(5, test_grp.size());
    {
        auto &grp_entry = test_grp.entry(0);
        EXPECT_EQ(17, grp_entry.get_int(fix44_gen::MDEntryType));
    }
    {
        auto &grp_entry = test_grp.entry(1);
        EXPECT_EQ(27, grp_entry.get_int(fix44_gen::MDEntryType));
    }
    {
        auto &grp_entry = test_grp.entry(2);
        EXPECT_EQ(37, grp_entry.get_int(fix44_gen::MDEntryType));
    }
    {
        auto &grp_entry = test_grp.entry(3);
        EXPECT_EQ(47, grp_entry.get_int(fix44_gen::MDEntryType));
    }
    {
        auto &grp_entry = test_grp.entry(4);
        EXPECT_EQ(57, grp_entry.get_int(fix44_gen::MDEntryType));
    }

}


TEST(TestFIXGroup, create_TestGrp1_multiple_tags){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag1, dict_fixTest);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fixTest_gen::GrpSizeTag1, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag1, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    auto &entry = test_grp.add_entry();
    EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::GrpEntryTag1));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::GrpEntryTag1));
    EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag1, 123));
    EXPECT_EQ(123, entry.get_int(fixTest_gen::GrpEntryTag1));

    EXPECT_EQ(FIXTagType::BOOL, entry.tag_type(fixTest_gen::BoolTag1));
    EXPECT_FALSE(entry.get_bool(fixTest_gen::BoolTag1));
    EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag1, true));
    EXPECT_TRUE(entry.get_bool(fixTest_gen::BoolTag1));

    EXPECT_EQ(FIXTagType::DOUBLE, entry.tag_type(fixTest_gen::DoubleTag1));
    EXPECT_EQ(0, entry.get_double(fixTest_gen::DoubleTag1).to_string().size());
    EXPECT_TRUE(entry.set_double(fixTest_gen::DoubleTag1, {"123.456"}));
    EXPECT_EQ("123.456", entry.get_double(fixTest_gen::DoubleTag1).to_string());

    EXPECT_EQ(FIXTagType::STRING, entry.tag_type(fixTest_gen::StringTag1));
    EXPECT_EQ(0, entry.get_string(fixTest_gen::StringTag1).to_string().size());
    EXPECT_TRUE(entry.set_string(fixTest_gen::StringTag1, {"Some Test value"}));
    EXPECT_EQ("Some Test value", entry.get_string(fixTest_gen::StringTag1).to_string());

    EXPECT_EQ(FIXTagType::DATE, entry.tag_type(fixTest_gen::DateTag1));
    EXPECT_EQ(0, entry.get_date(fixTest_gen::DateTag1).to_string().size());
    EXPECT_TRUE(entry.set_date(fixTest_gen::DateTag1, {"20221029"}));
    EXPECT_EQ("20221029", entry.get_date(fixTest_gen::DateTag1).to_string());

    EXPECT_EQ(FIXTagType::DATETIME, entry.tag_type(fixTest_gen::DatetimeTag1));
    EXPECT_EQ(0, entry.get_datetime(fixTest_gen::DatetimeTag1).to_string().size());
    EXPECT_TRUE(entry.set_datetime(fixTest_gen::DatetimeTag1, {"20221029 10:56:34.123456"}));
    EXPECT_EQ("20221029 10:56:34.123456", entry.get_datetime(fixTest_gen::DatetimeTag1).to_string());

    EXPECT_EQ(FIXTagType::RAWDATALEN, entry.tag_type(fixTest_gen::RawLenTag1));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::RawLenTag1));
    EXPECT_TRUE(entry.set_int(fixTest_gen::RawLenTag1, 100));
    EXPECT_EQ(100, entry.get_int(fixTest_gen::RawLenTag1));

    EXPECT_EQ(FIXTagType::RAWDATA, entry.tag_type(fixTest_gen::RawDataTag1));
    EXPECT_EQ(0, entry.get_rawdata(fixTest_gen::RawDataTag1).to_string().size());
    EXPECT_TRUE(entry.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002Some test value\001\002"}));
    EXPECT_EQ("\001\002Some test value\001\002", entry.get_rawdata(fixTest_gen::RawDataTag1).to_string());
    EXPECT_EQ(1, test_grp.size());
}

TEST(TestFIXGroup, create_TestGrp2_multiple_blocks){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag2, dict_fixTest);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fixTest_gen::GrpSizeTag2, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag2, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    auto &entry = test_grp.add_entry();
    EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::GrpEntryTag2));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::GrpEntryTag2));
    EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag2, 123));
    EXPECT_EQ(123, entry.get_int(fixTest_gen::GrpEntryTag2));

    // tags from 1st block
    EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::IntTag1));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::IntTag1));
    EXPECT_TRUE(entry.set_int(fixTest_gen::IntTag1, -323));
    EXPECT_EQ(-323, entry.get_int(fixTest_gen::IntTag1));

    EXPECT_EQ(FIXTagType::BOOL, entry.tag_type(fixTest_gen::BoolTag1));
    EXPECT_FALSE(entry.get_bool(fixTest_gen::BoolTag1));
    EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag1, true));
    EXPECT_TRUE(entry.get_bool(fixTest_gen::BoolTag1));

    EXPECT_EQ(FIXTagType::DOUBLE, entry.tag_type(fixTest_gen::DoubleTag1));
    EXPECT_EQ(0, entry.get_double(fixTest_gen::DoubleTag1).to_string().size());
    EXPECT_TRUE(entry.set_double(fixTest_gen::DoubleTag1, {"123.456"}));
    EXPECT_EQ("123.456", entry.get_double(fixTest_gen::DoubleTag1).to_string());

    EXPECT_EQ(FIXTagType::STRING, entry.tag_type(fixTest_gen::StringTag1));
    EXPECT_EQ(0, entry.get_string(fixTest_gen::StringTag1).to_string().size());
    EXPECT_TRUE(entry.set_string(fixTest_gen::StringTag1, {"Some Test value"}));
    EXPECT_EQ("Some Test value", entry.get_string(fixTest_gen::StringTag1).to_string());

    EXPECT_EQ(FIXTagType::DATE, entry.tag_type(fixTest_gen::DateTag1));
    EXPECT_EQ(0, entry.get_date(fixTest_gen::DateTag1).to_string().size());
    EXPECT_TRUE(entry.set_date(fixTest_gen::DateTag1, {"20221029"}));
    EXPECT_EQ("20221029", entry.get_date(fixTest_gen::DateTag1).to_string());

    EXPECT_EQ(FIXTagType::DATETIME, entry.tag_type(fixTest_gen::DatetimeTag1));
    EXPECT_EQ(0, entry.get_datetime(fixTest_gen::DatetimeTag1).to_string().size());
    EXPECT_TRUE(entry.set_datetime(fixTest_gen::DatetimeTag1, {"20221029 10:56:34.123456"}));
    EXPECT_EQ("20221029 10:56:34.123456", entry.get_datetime(fixTest_gen::DatetimeTag1).to_string());

    EXPECT_EQ(FIXTagType::RAWDATALEN, entry.tag_type(fixTest_gen::RawLenTag1));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::RawLenTag1));
    EXPECT_TRUE(entry.set_int(fixTest_gen::RawLenTag1, 100));
    EXPECT_EQ(100, entry.get_int(fixTest_gen::RawLenTag1));

    EXPECT_EQ(FIXTagType::RAWDATA, entry.tag_type(fixTest_gen::RawDataTag1));
    EXPECT_EQ(0, entry.get_rawdata(fixTest_gen::RawDataTag1).to_string().size());
    EXPECT_TRUE(entry.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002Some test value\001\002"}));
    EXPECT_EQ("\001\002Some test value\001\002", entry.get_rawdata(fixTest_gen::RawDataTag1).to_string());

    // tags from 2st block
    EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::IntTag2));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::IntTag2));
    EXPECT_TRUE(entry.set_int(fixTest_gen::IntTag2, 223));
    EXPECT_EQ(223, entry.get_int(fixTest_gen::IntTag2));

    EXPECT_EQ(FIXTagType::BOOL, entry.tag_type(fixTest_gen::BoolTag2));
    EXPECT_FALSE(entry.get_bool(fixTest_gen::BoolTag2));
    EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag2, false));
    EXPECT_FALSE(entry.get_bool(fixTest_gen::BoolTag2));
    EXPECT_EQ(1, test_grp.size());

    {
        auto &entry = test_grp.add_entry();
        EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::GrpEntryTag2));
        EXPECT_EQ(0, entry.get_int(fixTest_gen::GrpEntryTag2));
        EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag2, 1123));
        EXPECT_EQ(1123, entry.get_int(fixTest_gen::GrpEntryTag2));

        // tags from 2st block
        EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::IntTag2));
        EXPECT_EQ(0, entry.get_int(fixTest_gen::IntTag2));
        EXPECT_TRUE(entry.set_int(fixTest_gen::IntTag2, 1223));
        EXPECT_EQ(1223, entry.get_int(fixTest_gen::IntTag2));

        EXPECT_EQ(FIXTagType::BOOL, entry.tag_type(fixTest_gen::BoolTag2));
        EXPECT_FALSE(entry.get_bool(fixTest_gen::BoolTag2));
        EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag2, false));
        EXPECT_FALSE(entry.get_bool(fixTest_gen::BoolTag2));
        EXPECT_EQ(2, test_grp.size());
    }
}

TEST(TestFIXGroup, create_TestGrp3_nested_groups){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag3, dict_fixTest);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fixTest_gen::GrpSizeTag3, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag3, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    auto &entry = test_grp.add_entry();
    EXPECT_EQ(FIXTagType::INT, entry.tag_type(fixTest_gen::GrpEntryTag3));
    EXPECT_EQ(0, entry.get_int(fixTest_gen::GrpEntryTag3));
    EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag3, 123));
    EXPECT_EQ(123, entry.get_int(fixTest_gen::GrpEntryTag3));

    {// set 1st sub group
        EXPECT_TRUE(entry.has_tag(fixTest_gen::GrpSizeTag1));
        auto &nested_grp1 = entry.get_group(fixTest_gen::GrpSizeTag1);

        auto &nest_grp1_entry1 = nested_grp1.add_entry();
        EXPECT_EQ(FIXTagType::INT, nest_grp1_entry1.tag_type(fixTest_gen::GrpEntryTag1));
        EXPECT_EQ(0, nest_grp1_entry1.get_int(fixTest_gen::GrpEntryTag1));
        EXPECT_TRUE(nest_grp1_entry1.set_int(fixTest_gen::GrpEntryTag1, 111));
        EXPECT_EQ(111, nest_grp1_entry1.get_int(fixTest_gen::GrpEntryTag1));

        EXPECT_EQ(FIXTagType::STRING, nest_grp1_entry1.tag_type(fixTest_gen::StringTag1));
        EXPECT_EQ(0, nest_grp1_entry1.get_string(fixTest_gen::StringTag1).to_string().size());
        EXPECT_TRUE(nest_grp1_entry1.set_string(fixTest_gen::StringTag1, {"Nested grp1 entry1"}));
        EXPECT_EQ("Nested grp1 entry1", nest_grp1_entry1.get_string(fixTest_gen::StringTag1).to_string());
        EXPECT_EQ(1, nested_grp1.size());

        auto &nest_grp1_entry2 = nested_grp1.add_entry();
        EXPECT_EQ(FIXTagType::INT, nest_grp1_entry2.tag_type(fixTest_gen::GrpEntryTag1));
        EXPECT_EQ(0, nest_grp1_entry2.get_int(fixTest_gen::GrpEntryTag1));
        EXPECT_TRUE(nest_grp1_entry2.set_int(fixTest_gen::GrpEntryTag1, 112));
        EXPECT_EQ(112, nest_grp1_entry2.get_int(fixTest_gen::GrpEntryTag1));

        EXPECT_EQ(FIXTagType::STRING, nest_grp1_entry2.tag_type(fixTest_gen::StringTag1));
        EXPECT_EQ(0, nest_grp1_entry2.get_string(fixTest_gen::StringTag1).to_string().size());
        EXPECT_TRUE(nest_grp1_entry2.set_string(fixTest_gen::StringTag1, {"Nested grp1 entry2"}));
        EXPECT_EQ("Nested grp1 entry2", nest_grp1_entry2.get_string(fixTest_gen::StringTag1).to_string());
        EXPECT_EQ(2, nested_grp1.size());
    }

    {// set 2st sub group
        EXPECT_TRUE(entry.has_tag(fixTest_gen::GrpSizeTag2));
        auto &nested_grp2 = entry.get_group(fixTest_gen::GrpSizeTag2);

        auto &nest_grp2_entry1 = nested_grp2.add_entry();
        EXPECT_EQ(FIXTagType::INT, nest_grp2_entry1.tag_type(fixTest_gen::GrpEntryTag2));
        EXPECT_EQ(0, nest_grp2_entry1.get_int(fixTest_gen::GrpEntryTag2));
        EXPECT_TRUE(nest_grp2_entry1.set_int(fixTest_gen::GrpEntryTag2, 121));
        EXPECT_EQ(121, nest_grp2_entry1.get_int(fixTest_gen::GrpEntryTag2));

        EXPECT_EQ(FIXTagType::INT, nest_grp2_entry1.tag_type(fixTest_gen::IntTag2));
        EXPECT_EQ(0, nest_grp2_entry1.get_int(fixTest_gen::IntTag2));
        EXPECT_TRUE(nest_grp2_entry1.set_int(fixTest_gen::IntTag2, 721));
        EXPECT_EQ(721, nest_grp2_entry1.get_int(fixTest_gen::IntTag2));
        EXPECT_EQ(1, nested_grp2.size());

        auto &nest_grp2_entry2 = nested_grp2.add_entry();
        EXPECT_EQ(FIXTagType::INT, nest_grp2_entry2.tag_type(fixTest_gen::GrpEntryTag2));
        EXPECT_EQ(0, nest_grp2_entry2.get_int(fixTest_gen::GrpEntryTag2));
        EXPECT_TRUE(nest_grp2_entry2.set_int(fixTest_gen::GrpEntryTag2, 122));
        EXPECT_EQ(122, nest_grp2_entry2.get_int(fixTest_gen::GrpEntryTag2));

        EXPECT_EQ(FIXTagType::INT, nest_grp2_entry2.tag_type(fixTest_gen::IntTag2));
        EXPECT_EQ(0, nest_grp2_entry2.get_int(fixTest_gen::IntTag2));
        EXPECT_TRUE(nest_grp2_entry2.set_int(fixTest_gen::IntTag2, 722));
        EXPECT_EQ(722, nest_grp2_entry2.get_int(fixTest_gen::IntTag2));
        EXPECT_EQ(2, nested_grp2.size());
    }
    EXPECT_EQ(1, test_grp.size());
}

TEST(TestFIXGroup, serialize_TestGrp1_multiple_tags){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag1, dict_fixTest);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fixTest_gen::GrpSizeTag1, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag1, test_grp.start_entry_tag());

    auto &entry = test_grp.add_entry();
    EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag1, 123));
    EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag1, true));
    EXPECT_TRUE(entry.set_double(fixTest_gen::DoubleTag1, {"123.456"}));
    EXPECT_TRUE(entry.set_string(fixTest_gen::StringTag1, {"Some Test value"}));
    EXPECT_TRUE(entry.set_date(fixTest_gen::DateTag1, {"20221029"}));
    EXPECT_TRUE(entry.set_datetime(fixTest_gen::DatetimeTag1, {"20221029 10:56:34.123456"}));
    EXPECT_TRUE(entry.set_int(fixTest_gen::RawLenTag1, 100));
    EXPECT_TRUE(entry.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002Some test value\001\002"}));
    EXPECT_EQ(1, test_grp.size());

    std::vector<char> buffer;
    test_grp.serialize(buffer);
    EXPECT_EQ(115, buffer.size());
    EXPECT_EQ("28=1\x1" "29=123\x1" "4=Y\x1" "7=123.456\x1" "13=Some Test value\x1" "16=20221029\x1" 
            "19=20221029 10:56:34.123456\x1" "22=100\x1" "23=\x1\x2Some test value\x1\x2\x1", std::string(buffer.begin(), buffer.end()));
}

TEST(TestFIXGroup, serialize_TestGrp2_multiple_blocks){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag2, dict_fixTest);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fixTest_gen::GrpSizeTag2, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag2, test_grp.start_entry_tag());

    {
        auto &entry = test_grp.add_entry();
        EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag2, 123));

        // tags from 1st block
        EXPECT_TRUE(entry.set_int(fixTest_gen::IntTag1, -323));
        EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag1, true));
        EXPECT_TRUE(entry.set_double(fixTest_gen::DoubleTag1, {"123.456"}));
        EXPECT_TRUE(entry.set_string(fixTest_gen::StringTag1, {"Some Test value"}));
        EXPECT_TRUE(entry.set_date(fixTest_gen::DateTag1, {"20221029"}));
        EXPECT_TRUE(entry.set_datetime(fixTest_gen::DatetimeTag1, {"20221029 10:56:34.123456"}));
        EXPECT_TRUE(entry.set_int(fixTest_gen::RawLenTag1, 100));
        EXPECT_TRUE(entry.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002Some test value\001\002"}));

        // tags from 2st block
        EXPECT_TRUE(entry.set_int(fixTest_gen::IntTag2, 223));
        EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag2, false));
        EXPECT_EQ(1, test_grp.size());
    }
    {
        auto &entry = test_grp.add_entry();
        EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag2, 1123));
        // tags from 2st block
        EXPECT_TRUE(entry.set_int(fixTest_gen::IntTag2, 1223));
        EXPECT_TRUE(entry.set_bool(fixTest_gen::BoolTag2, false));
        EXPECT_EQ(2, test_grp.size());
    }
    std::vector<char> buffer;
    test_grp.serialize(buffer);
    EXPECT_EQ(151, buffer.size());
    EXPECT_EQ("30=2\x1" "31=123\x1" "1=-323\x1" "4=Y\x1" "7=123.456\x1" "13=Some Test value\x1" "16=20221029\x1" 
        "19=20221029 10:56:34.123456\x1" "22=100\x1" "23=\x1\x2Some test value\x1\x2\x1" "2=223\x1" "5=N\x1" 
        "31=1123\x1" "2=1223\x1" "5=N\x1", std::string(buffer.begin(), buffer.end()));
}

TEST(TestFIXGroup, serialize_TestGrp3_nested_groups){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag3, dict_fixTest);
    fix::FIXGroup test_grp = meta_grp.create();
    EXPECT_EQ(fixTest_gen::GrpSizeTag3, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag3, test_grp.start_entry_tag());
    EXPECT_EQ(0, test_grp.size());

    auto &entry = test_grp.add_entry();
    EXPECT_TRUE(entry.set_int(fixTest_gen::GrpEntryTag3, 123));

    {// set 1st sub group
        EXPECT_TRUE(entry.has_tag(fixTest_gen::GrpSizeTag1));
        auto &nested_grp1 = entry.get_group(fixTest_gen::GrpSizeTag1);

        auto &nest_grp1_entry1 = nested_grp1.add_entry();
        EXPECT_TRUE(nest_grp1_entry1.set_int(fixTest_gen::GrpEntryTag1, 111));
        EXPECT_TRUE(nest_grp1_entry1.set_string(fixTest_gen::StringTag1, {"Nested grp1 entry1"}));
        EXPECT_EQ(1, nested_grp1.size());

        auto &nest_grp1_entry2 = nested_grp1.add_entry();
        EXPECT_TRUE(nest_grp1_entry2.set_int(fixTest_gen::GrpEntryTag1, 112));
        EXPECT_TRUE(nest_grp1_entry2.set_string(fixTest_gen::StringTag1, {"Nested grp1 entry2"}));
        EXPECT_EQ(2, nested_grp1.size());
    }

    {// set 2st sub group
        EXPECT_TRUE(entry.has_tag(fixTest_gen::GrpSizeTag2));
        auto &nested_grp2 = entry.get_group(fixTest_gen::GrpSizeTag2);

        auto &nest_grp2_entry1 = nested_grp2.add_entry();
        EXPECT_TRUE(nest_grp2_entry1.set_int(fixTest_gen::GrpEntryTag2, 121));
        EXPECT_TRUE(nest_grp2_entry1.set_int(fixTest_gen::IntTag2, 721));
        EXPECT_EQ(1, nested_grp2.size());

        auto &nest_grp2_entry2 = nested_grp2.add_entry();
        EXPECT_TRUE(nest_grp2_entry2.set_int(fixTest_gen::GrpEntryTag2, 122));
        EXPECT_TRUE(nest_grp2_entry2.set_int(fixTest_gen::IntTag2, 722));
        EXPECT_EQ(2, nested_grp2.size());
    }
    EXPECT_EQ(1, test_grp.size());

    std::vector<char> buffer;
    test_grp.serialize(buffer);
    EXPECT_EQ(106, buffer.size());
    EXPECT_EQ("32=1\x1" "33=123\x1" "28=2\x1" "29=111\x1" "13=Nested grp1 entry1\x1" "29=112\x1" "13=Nested grp1 entry2\x1" 
            "30=2\x1" "31=121\x1" "2=721\x1" "31=122\x1" "2=722\x1", std::string(buffer.begin(), buffer.end()));
}

TEST(TestFIXGroup, parse_TestGrp1_multiple_tags){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag1, dict_fixTest);
    // added tag 777 to the end to finish parsing
    MsgReceived msg_data{"28=1\00129=123\0014=Y\0017=123.456\00113=Some Test value\00116=20221029\001"
              "19=20221029 10:56:34.123456\00122=19\00123=\001\002Some test value\001\002\001777=111\001", 0, "", 0}; 

    fix::FIXGroup test_grp = meta_grp.parse(msg_data);
    EXPECT_TRUE(msg_data.error_.empty());
    EXPECT_EQ(fixTest_gen::GrpSizeTag1, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag1, test_grp.start_entry_tag());
    EXPECT_EQ(1, test_grp.size());
    auto &grp_entry = test_grp.entry(0);    
    EXPECT_EQ(123, grp_entry.get_int(fixTest_gen::GrpEntryTag1));
    EXPECT_TRUE(grp_entry.get_bool(fixTest_gen::BoolTag1));
    EXPECT_EQ("123.456", grp_entry.get_double(fixTest_gen::DoubleTag1).to_string());
    EXPECT_EQ("Some Test value", grp_entry.get_string(fixTest_gen::StringTag1).to_string());
    EXPECT_EQ("20221029", grp_entry.get_date(fixTest_gen::DateTag1).to_string());
    EXPECT_EQ("20221029 10:56:34.123456", grp_entry.get_datetime(fixTest_gen::DatetimeTag1).to_string());
    EXPECT_EQ(19, grp_entry.get_int(fixTest_gen::RawLenTag1));
    EXPECT_EQ("\001\002Some test value\001\002", grp_entry.get_rawdata(fixTest_gen::RawDataTag1).to_string());
}

TEST(TestFIXGroup, parse_TestGrp2_multiple_blocks){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag2, dict_fixTest);
    // added tag 777 to the end to finish parsing
    MsgReceived msg_data{"30=2\00131=123\0011=-323\0014=Y\0017=123.456\00113=Some Test value\00116=20221029\001"
              "19=20221029 10:56:34.123456\00122=19\00123=\001\002Some test value\001\002\0012=223\0015=N\001"
              "31=1123\0012=1223\0015=N\001777=111\001", 0, "", 0}; 

    fix::FIXGroup test_grp = meta_grp.parse(msg_data);
    EXPECT_TRUE(msg_data.error_.empty());
    EXPECT_EQ(fixTest_gen::GrpSizeTag2, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag2, test_grp.start_entry_tag());
    EXPECT_EQ(2, test_grp.size());
    {
        auto &grp_entry = test_grp.entry(0);    
        EXPECT_EQ(123, grp_entry.get_int(fixTest_gen::GrpEntryTag2));
        EXPECT_EQ(-323, grp_entry.get_int(fixTest_gen::IntTag1));
        EXPECT_TRUE(grp_entry.get_bool(fixTest_gen::BoolTag1));
        EXPECT_EQ("123.456", grp_entry.get_double(fixTest_gen::DoubleTag1).to_string());
        EXPECT_EQ("Some Test value", grp_entry.get_string(fixTest_gen::StringTag1).to_string());
        EXPECT_EQ("20221029", grp_entry.get_date(fixTest_gen::DateTag1).to_string());
        EXPECT_EQ("20221029 10:56:34.123456", grp_entry.get_datetime(fixTest_gen::DatetimeTag1).to_string());
        EXPECT_EQ(19, grp_entry.get_int(fixTest_gen::RawLenTag1));
        EXPECT_EQ("\001\002Some test value\001\002", grp_entry.get_rawdata(fixTest_gen::RawDataTag1).to_string());

        EXPECT_EQ(223, grp_entry.get_int(fixTest_gen::IntTag2));
        EXPECT_FALSE(grp_entry.get_bool(fixTest_gen::BoolTag2));
    }
    {
        auto &grp_entry = test_grp.entry(1);    
        EXPECT_EQ(1123, grp_entry.get_int(fixTest_gen::GrpEntryTag2));
        EXPECT_EQ(1223, grp_entry.get_int(fixTest_gen::IntTag2));
        EXPECT_FALSE(grp_entry.get_bool(fixTest_gen::BoolTag2));
    }
}


TEST(TestFIXGroup, parse_TestGrp3_nested_groups){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXGroupMetadata meta_grp(fixTest_gen::GrpSizeTag3, dict_fixTest);
    // added tag 777 to the end to finish parsing
    MsgReceived msg_data{"32=1\00133=123\00128=2\00129=111\00113=Nested grp1 entry1\00129=112\001"
              "13=Nested grp1 entry2\00130=2\00131=121\0012=721\00131=122\0012=722\001777=111\001", 0, "", 0}; 

    fix::FIXGroup test_grp = meta_grp.parse(msg_data);
    EXPECT_TRUE(msg_data.error_.empty());
    EXPECT_EQ(fixTest_gen::GrpSizeTag3, test_grp.leading_tag());
    EXPECT_EQ(fixTest_gen::GrpEntryTag3, test_grp.start_entry_tag());
    EXPECT_EQ(1, test_grp.size());
    {
        auto &grp_entry = test_grp.entry(0);    
        EXPECT_EQ(123, grp_entry.get_int(fixTest_gen::GrpEntryTag3));
        EXPECT_TRUE(grp_entry.has_tag(fixTest_gen::GrpSizeTag1));
        auto &nested_grp1 = grp_entry.get_group(fixTest_gen::GrpSizeTag1);
        EXPECT_EQ(2, nested_grp1.size());
        {
            auto &nest_grp1_entry1 = nested_grp1.entry(0);    
            EXPECT_EQ(111, nest_grp1_entry1.get_int(fixTest_gen::GrpEntryTag1));
            EXPECT_EQ("Nested grp1 entry1", nest_grp1_entry1.get_string(fixTest_gen::StringTag1).to_string());
        }
        {
            auto &nest_grp1_entry2 = nested_grp1.entry(1);    
            EXPECT_EQ(112, nest_grp1_entry2.get_int(fixTest_gen::GrpEntryTag1));
            EXPECT_EQ("Nested grp1 entry2", nest_grp1_entry2.get_string(fixTest_gen::StringTag1).to_string());
        }

        EXPECT_TRUE(grp_entry.has_tag(fixTest_gen::GrpSizeTag2));
        auto &nested_grp2 = grp_entry.get_group(fixTest_gen::GrpSizeTag2);
        EXPECT_EQ(2, nested_grp2.size());
        {
            auto &nest_grp2_entry1 = nested_grp2.entry(0);    
            EXPECT_EQ(121, nest_grp2_entry1.get_int(fixTest_gen::GrpEntryTag2));
            EXPECT_EQ(721, nest_grp2_entry1.get_int(fixTest_gen::IntTag2));
        }
        {
            auto &nest_grp2_entry2 = nested_grp2.entry(1);    
            EXPECT_EQ(122, nest_grp2_entry2.get_int(fixTest_gen::GrpEntryTag2));
            EXPECT_EQ(722, nest_grp2_entry2.get_int(fixTest_gen::IntTag2));
        }
    }
}
