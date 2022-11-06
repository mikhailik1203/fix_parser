#include <gtest/gtest.h>
#include "../src/FIXMessage.hpp"
#include "../src/dictionary/FIX44Builder.hpp"
#include "FIXTestBuilder.hpp"

using namespace fix;

TEST(TestFIXMessage, create_vanilla){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXMessageMetadata meta_msg("V", dict_fix44);
    fix::FIXMessage test_msg = meta_msg.create();
    EXPECT_EQ(FIXProtocol::FIX44, test_msg.protocol());
    EXPECT_EQ(FIXMessageType::MarketDataRequest, test_msg.message_type().type());
}

TEST(TestFIXMessage, serialize_empty){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXMessageMetadata meta_msg("V", dict_fix44);
    fix::FIXMessage test_msg = meta_msg.create();
    EXPECT_EQ(FIXProtocol::FIX44, test_msg.protocol());
    EXPECT_EQ(FIXMessageType::MarketDataRequest, test_msg.message_type().type());

    MsgSerialised buffer(128, 0);
    test_msg.serialize(buffer);
    EXPECT_EQ(5, buffer.pos_); //msg contains 35=V<SOH>
    EXPECT_EQ("35=V\001", buffer.to_string());
}

TEST(TestFIXMessage, serialize_message){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXMessageMetadata meta_msg("V", dict_fix44);
    fix::FIXMessage test_msg = meta_msg.create();
    EXPECT_EQ(FIXProtocol::FIX44, test_msg.protocol());
    EXPECT_EQ(FIXMessageType::MarketDataRequest, test_msg.message_type().type());

    {// fill tags
        test_msg.set_string(fix44_gen::MsgType, {"V"});
        test_msg.set_string(fix44_gen::SenderCompID, {"TestSndrCompId"});
        test_msg.set_string(fix44_gen::TargetCompID, {"TestTgtCompId"});
        test_msg.set_string(fix44_gen::MdReqId, {"TestMdReqId"});
        test_msg.set_int(fix44_gen::MsgSeqNum, 77);
        test_msg.set_int(fix44_gen::SubscriptionRequestType, 1);
        test_msg.set_int(fix44_gen::MarketDepth, 2);
        test_msg.set_int(fix44_gen::MDUpdateType, 3);
        test_msg.set_datetime(fix44_gen::SendingTime, {"20221005 12:44:55.123456"});
        {
            auto &md_grp = test_msg.get_group(fix44_gen::NoMdEntryTypes);
            {
                auto &md_entry = md_grp.add_entry();
                md_entry.set_int(fix44_gen::MDEntryType, 1);
            }
            {
                auto &md_entry = md_grp.add_entry();
                md_entry.set_int(fix44_gen::MDEntryType, 2);
            }
            {
                auto &md_entry = md_grp.add_entry();
                md_entry.set_int(fix44_gen::MDEntryType, 3);
            }
        }
        {
            auto &md_grp = test_msg.get_group(fix44_gen::NoRelatedSym);
            {
                auto &md_entry = md_grp.add_entry();
                md_entry.set_string(fix44_gen::Symbol, {"AAA"});
            }
            {
                auto &md_entry = md_grp.add_entry();
                md_entry.set_string(fix44_gen::Symbol, {"BBB"});
            }
            {
                auto &md_entry = md_grp.add_entry();
                md_entry.set_string(fix44_gen::Symbol, {"CCC"});
            }
        }
    }

    MsgSerialised buffer(256, 0);
    test_msg.serialize(buffer);
    std::string expected_msg = "35=V\00149=TestSndrCompId\00156=TestTgtCompId\00134=77\00152=20221005 12:44:55.123456\001262=TestMdReqId\001263=1\001"
            "264=2\001265=3\001267=3\001269=1\001269=2\001269=3\001146=3\00155=AAA\00155=BBB\00155=CCC\001";
    EXPECT_EQ(expected_msg.size(), buffer.pos_);
    EXPECT_EQ(expected_msg, buffer.to_string());
}

TEST(TestFIXMessage, parse_message){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXMessageMetadata meta_msg("V", dict_fix44);
    // message is parsed without 35 tag and till tag 10
    MsgReceived msg_data{"49=TestSndrCompId\00156=TestTgtCompId\00134=77\00152=20221005 12:44:55.123456\001262=TestMdReqId\001263=1\001"
            "264=2\001265=3\001267=3\001269=1\001269=2\001269=3\001146=3\00155=AAA\00155=BBB\00155=CCC\00110=123\0001", 0, "", 0};

    fix::FIXMessage test_msg = meta_msg.parse(msg_data);
    EXPECT_EQ("", msg_data.error_);
    EXPECT_EQ(FIXMessageType::MarketDataRequest, test_msg.message_type().type());
    EXPECT_EQ("TestSndrCompId", test_msg.get_string(fix44_gen::SenderCompID).to_string());
    EXPECT_EQ("TestTgtCompId", test_msg.get_string(fix44_gen::TargetCompID).to_string());
    EXPECT_EQ("TestMdReqId", test_msg.get_string(fix44_gen::MdReqId).to_string());
    EXPECT_EQ(77, test_msg.get_int(fix44_gen::MsgSeqNum));
    EXPECT_EQ(1, test_msg.get_int(fix44_gen::SubscriptionRequestType));
    EXPECT_EQ(2, test_msg.get_int(fix44_gen::MarketDepth));
    EXPECT_EQ(3, test_msg.get_int(fix44_gen::MDUpdateType));
    EXPECT_EQ("20221005 12:44:55.123456", test_msg.get_datetime(fix44_gen::SendingTime).to_string());

    auto &md_grp = test_msg.get_group(fix44_gen::NoMdEntryTypes);
    EXPECT_EQ(3, md_grp.size());
    {
        const auto &entry = md_grp.entry(0);
        EXPECT_EQ(1, entry.get_int(fix44_gen::MDEntryType));        
    }
    {
        const auto &entry = md_grp.entry(1);
        EXPECT_EQ(2, entry.get_int(fix44_gen::MDEntryType));
    }
    {
        const auto &entry = md_grp.entry(2);
        EXPECT_EQ(3, entry.get_int(fix44_gen::MDEntryType));
    }

    auto &sbl_grp = test_msg.get_group(fix44_gen::NoRelatedSym);
    EXPECT_EQ(3, sbl_grp.size());
    {
        const auto &entry = sbl_grp.entry(0);
        EXPECT_EQ("AAA", entry.get_string(fix44_gen::Symbol).to_string());
    }
    {
        const auto &entry = sbl_grp.entry(1);
        EXPECT_EQ("BBB", entry.get_string(fix44_gen::Symbol).to_string());
    }
    {
        const auto &entry = sbl_grp.entry(2);
        EXPECT_EQ("CCC", entry.get_string(fix44_gen::Symbol).to_string());
    }
}


TEST(TestFIXMessage, create_UT_messages){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    {
        fix::FIXMessageMetadata meta_msg("UT1", dict_fixTest);
        fix::FIXMessage test_msg = meta_msg.create();
        EXPECT_EQ(FIXProtocol::FIXTest, test_msg.protocol());
        EXPECT_EQ(FIXMessageType::UserDefined, test_msg.message_type().type());
        EXPECT_TRUE(test_msg.support_tag(fixTest_gen::IntTag1));
        EXPECT_FALSE(test_msg.has_tag(fixTest_gen::IntTag1));
    }

    {
        fix::FIXMessageMetadata meta_msg("UT2", dict_fixTest);
        fix::FIXMessage test_msg = meta_msg.create();
        EXPECT_EQ(FIXProtocol::FIXTest, test_msg.protocol());
        EXPECT_EQ(FIXMessageType::UserDefined, test_msg.message_type().type());
    }

    {
        fix::FIXMessageMetadata meta_msg("UT3", dict_fixTest);
        fix::FIXMessage test_msg = meta_msg.create();
        EXPECT_EQ(FIXProtocol::FIXTest, test_msg.protocol());
        EXPECT_EQ(FIXMessageType::UserDefined, test_msg.message_type().type());
    }

}

TEST(TestFIXMessage, serialize_UT1_message){
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
        test_msg.set_int(fixTest_gen::RawLenTag1, 100);
        test_msg.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002raw data \001\002"});
    }

    MsgSerialised buffer(128, 0);
    test_msg.serialize(buffer);
    std::string expected_msg = "35=UT1\x1" "1=111\x1" "4=Y\x1" "7=123.456\x1" "13=Test string value\x1" 
                "16=20221024\x1" "19=20221024 12:45:56\x1" "22=100\x1" "23=\x1\x2raw data \x1\x2\x1";
    EXPECT_EQ(expected_msg.size(), buffer.pos_);
    EXPECT_EQ(expected_msg, buffer.to_string());
}

TEST(TestFIXMessage, serialize_UT2_message){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXMessageMetadata meta_msg("UT2", dict_fixTest);
    fix::FIXMessage test_msg = meta_msg.create();
    EXPECT_EQ(FIXProtocol::FIXTest, test_msg.protocol());
    EXPECT_EQ(FIXMessageType::UserDefined, test_msg.message_type().type());

    {// fill tags for 1st block
        test_msg.set_int(fixTest_gen::IntTag1, 222);
        test_msg.set_bool(fixTest_gen::BoolTag1, true);
        test_msg.set_double(fixTest_gen::DoubleTag1, {"223.456"});
        test_msg.set_string(fixTest_gen::StringTag1, {"Test string value"});
        test_msg.set_date(fixTest_gen::DateTag1, {"20221124"});
        test_msg.set_datetime(fixTest_gen::DatetimeTag1, {"20221124 12:45:56"});
        test_msg.set_int(fixTest_gen::RawLenTag1, 100);
        test_msg.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002raw data \001\002"});
    }
    {// fill tags for 2st block
        auto &grp1 = test_msg.get_group(fixTest_gen::GrpSizeTag1);
        {
            auto &entry1 = grp1.add_entry();
            entry1.set_int(fixTest_gen::GrpEntryTag1, 1);
            entry1.set_bool(fixTest_gen::BoolTag1, true);
            entry1.set_double(fixTest_gen::DoubleTag1, {"223.456"});
            entry1.set_string(fixTest_gen::StringTag1, {"Test string value111"});
            entry1.set_date(fixTest_gen::DateTag1, {"20221122"});
            entry1.set_datetime(fixTest_gen::DatetimeTag1, {"20221122 12:45:56"});
            entry1.set_int(fixTest_gen::RawLenTag1, 100);
            entry1.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002raw 111 data \001\002"});

            auto &entry2 = grp1.add_entry();
            entry2.set_int(fixTest_gen::GrpEntryTag1, 2);
            entry2.set_bool(fixTest_gen::BoolTag1, false);
            entry2.set_double(fixTest_gen::DoubleTag1, {"113.456"});
            entry2.set_string(fixTest_gen::StringTag1, {"Test string value222"});
            entry2.set_date(fixTest_gen::DateTag1, {"20221111"});
            entry2.set_datetime(fixTest_gen::DatetimeTag1, {"20221111 12:45:56"});
            entry2.set_int(fixTest_gen::RawLenTag1, 100);
            entry2.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002raw 222 data \001\002"});
        }
        test_msg.set_int(fixTest_gen::IntTag2, 2222);
        test_msg.set_bool(fixTest_gen::BoolTag2, false);
        test_msg.set_double(fixTest_gen::DoubleTag2, {"2223.456"});
        
        auto &grp2 = test_msg.get_group(fixTest_gen::GrpSizeTag2);
        {
            auto &entry1 = grp2.add_entry();
            entry1.set_int(fixTest_gen::GrpEntryTag2, 1122);
            entry1.set_int(fixTest_gen::IntTag1, 1000);
            entry1.set_bool(fixTest_gen::BoolTag1, true);
            entry1.set_double(fixTest_gen::DoubleTag1, {"223.456"});
            entry1.set_string(fixTest_gen::StringTag1, {"Test string value111"});
            entry1.set_date(fixTest_gen::DateTag1, {"20221122"});
            entry1.set_datetime(fixTest_gen::DatetimeTag1, {"20221122 12:45:56"});
            entry1.set_int(fixTest_gen::RawLenTag1, 100);
            entry1.set_rawdata(fixTest_gen::RawDataTag1, {"\001\002raw 111 data \001\002"});

            entry1.set_int(fixTest_gen::IntTag2, 2000);
            entry1.set_bool(fixTest_gen::BoolTag2, false);
        }

        test_msg.set_string(fixTest_gen::StringTag2, {"Test string value2"});
        test_msg.set_date(fixTest_gen::DateTag2, {"20221124"});
        test_msg.set_datetime(fixTest_gen::DatetimeTag2, {"20221124 12:45:56"});
        test_msg.set_int(fixTest_gen::RawLenTag2, 111);
        test_msg.set_rawdata(fixTest_gen::RawDataTag2, {"\001\002raw data222 \001\002"});
        
        auto &grp3 = test_msg.get_group(fixTest_gen::GrpSizeTag3);
        {
            auto &entry1 = grp3.add_entry();
            entry1.set_int(fixTest_gen::GrpEntryTag3, 1122);
            auto &sub_grp1 = entry1.get_group(fixTest_gen::GrpSizeTag1);
            auto &entry11 = sub_grp1.add_entry();
            entry11.set_int(fixTest_gen::GrpEntryTag1, 1011);
        }
    }

    MsgSerialised buffer(1024, 0);
    test_msg.serialize(buffer);
    std::string expected_msg = "35=UT2\x1" 
        "1=222\x1" "4=Y\x1" "7=223.456\x1" "13=Test string value\x1" "16=20221124\x1" "19=20221124 12:45:56\x1" "22=100\x1" "23=\x1\x2raw data \x1\x2\x1" 
        "28=2\x1" "29=1\x1" "4=Y\x1" "7=223.456\x1" "13=Test string value111\x1" "16=20221122\x1" "19=20221122 12:45:56\x1" "22=100\x1" "23=\x1\x2raw 111 data \x1\x2\x1" 
                  "29=2\x1" "4=N\x1" "7=113.456\x1" "13=Test string value222\x1" "16=20221111\x1" "19=20221111 12:45:56\x1" "22=100\x1" "23=\x1\x2raw 222 data \x1\x2\x1" 
        "2=2222\x1" "5=N\x1" "11=2223.456\x1" 
            "30=1\x1" "31=1122\x1" "1=1000\x1" "4=Y\x1" "7=223.456\x1" "13=Test string value111\x1" "16=20221122\x1" "19=20221122 12:45:56\x1" "22=100\x1" "23=\x1\x2raw 111 data \x1\x2\x1" "2=2000\x1" "5=N\x1" 
            "14=Test string value2\x1" "17=20221124\x1" "20=20221124 12:45:56\x1" "24=111\x1" "25=\x1\x2raw data222 \x1\x2\x1" 
        "32=1\x1" "33=1122\x1" "28=1\x1" "29=1011\x1";
    EXPECT_EQ(expected_msg.size(), buffer.pos_);
    EXPECT_EQ(expected_msg, buffer.to_string());
}

TEST(TestFIXMessage, serialize_UT3_message){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXMessageMetadata meta_msg("UT3", dict_fixTest);
    fix::FIXMessage test_msg = meta_msg.create();
    EXPECT_EQ(FIXProtocol::FIXTest, test_msg.protocol());
    EXPECT_EQ(FIXMessageType::UserDefined, test_msg.message_type().type());

    {// fill tags for GrpSizeTag5 group
        auto &grp1 = test_msg.get_group(fixTest_gen::GrpSizeTag5);
        {
            auto &entry1 = grp1.add_entry();
            entry1.set_int(fixTest_gen::GrpEntryTag5, 110011);
            auto &entry2 = grp1.add_entry();
            entry2.set_int(fixTest_gen::GrpEntryTag5, 110022);
            auto &entry3 = grp1.add_entry();
            entry3.set_int(fixTest_gen::GrpEntryTag5, 110033);
        }
    }
    {// fill tags for GrpSizeTag4 group
        auto &grp1 = test_msg.get_group(fixTest_gen::GrpSizeTag4);
        {
            auto &entry1 = grp1.add_entry();
            entry1.set_int(fixTest_gen::GrpEntryTag4, 220011);

            auto &subgrp1 = entry1.get_group(fixTest_gen::GrpSizeTag3);
            { //add sub group GrpSizeTag3
                auto &entry1 = subgrp1.add_entry();
                entry1.set_int(fixTest_gen::GrpEntryTag3, 230011);
                auto &sub2grp1 = entry1.get_group(fixTest_gen::GrpSizeTag1);
                { //add sub group GrpSizeTag3
                    auto &entry1 = sub2grp1.add_entry();
                    entry1.set_int(fixTest_gen::GrpEntryTag1, 240011);
                    entry1.set_double(fixTest_gen::DoubleTag1, {"1100.24"});
                    entry1.set_string(fixTest_gen::StringTag1, {"Str Grp1->Grp3->Grp4, entry1"});
                    auto &entry2 = sub2grp1.add_entry();
                    entry2.set_int(fixTest_gen::GrpEntryTag1, 240012);
                    entry2.set_double(fixTest_gen::DoubleTag1, {"1200.24"});
                    entry2.set_string(fixTest_gen::StringTag1, {"Str Grp1->Grp3->Grp4, entry2"});
                }
                auto &sub2grp2 = entry1.get_group(fixTest_gen::GrpSizeTag2);
                { //add sub group GrpSizeTag2
                    auto &entry1 = sub2grp2.add_entry();
                    entry1.set_int(fixTest_gen::GrpEntryTag2, 250011);
                    entry1.set_int(fixTest_gen::IntTag1, 2500111);
                    entry1.set_int(fixTest_gen::IntTag2, 2500112);
                    entry1.set_bool(fixTest_gen::BoolTag2, true);
                    
                    auto &entry2 = sub2grp2.add_entry();
                    entry2.set_int(fixTest_gen::GrpEntryTag2, 250012);
                    entry2.set_int(fixTest_gen::IntTag1, 2500121);
                    entry2.set_int(fixTest_gen::IntTag2, 2500122);
                    entry2.set_bool(fixTest_gen::BoolTag2, false);
                }
            }
        }
    }

    MsgSerialised buffer(256, 0);
    test_msg.serialize(buffer);
    std::string expected_msg = "35=UT3\x1" 
        "37=3\x1" "38=110011\x1" "38=110022\x1" "38=110033\x1" 
        "34=1\x1" "36=220011\x1" "32=1\x1" "33=230011\x1" 
            "28=2\x1" "29=240011\x1" "7=1100.24\x1" "13=Str Grp1->Grp3->Grp4, entry1\x1" 
                      "29=240012\x1" "7=1200.24\x1" "13=Str Grp1->Grp3->Grp4, entry2\x1" 
            "30=2\x1" "31=250011\x1" "1=2500111\x1" "2=2500112\x1" "5=Y\x1" 
                      "31=250012\x1" "1=2500121\x1" "2=2500122\x1" "5=N\x1";
    EXPECT_EQ(expected_msg.size(), buffer.pos_);
    EXPECT_EQ(expected_msg, buffer.to_string());
}


TEST(TestFIXMessage, parse_UT1_message){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXMessageMetadata meta_msg("UT1", dict_fixTest);
    // message is parsed without 35 tag and till tag 777
    MsgReceived msg_data{"1=111\x1" "4=Y\x1" "7=123.456\x1" "13=Test string value\x1" "16=20221024\x1" 
            "19=20221024 12:45:56\x1" "22=13\x1" "23=\x1\x2raw data \x1\x2\x1" "777=111\x1", 0, "", 0};

    fix::FIXMessage test_msg = meta_msg.parse(msg_data);
    EXPECT_EQ("", msg_data.error_);

    EXPECT_EQ(111, test_msg.get_int(fixTest_gen::IntTag1));
    EXPECT_TRUE(test_msg.get_bool(fixTest_gen::BoolTag1));
    EXPECT_EQ("123.456", test_msg.get_double(fixTest_gen::DoubleTag1).to_string());
    EXPECT_EQ("Test string value", test_msg.get_string(fixTest_gen::StringTag1).to_string());
    EXPECT_EQ("20221024", test_msg.get_date(fixTest_gen::DateTag1).to_string());
    EXPECT_EQ("20221024 12:45:56", test_msg.get_datetime(fixTest_gen::DatetimeTag1).to_string());
    EXPECT_EQ(13, test_msg.get_int(fixTest_gen::RawLenTag1));
    EXPECT_EQ("\001\002raw data \001\002", test_msg.get_rawdata(fixTest_gen::RawDataTag1).to_string());
}

TEST(TestFIXMessage, parse_UT2_message){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXMessageMetadata meta_msg("UT2", dict_fixTest);
    // message is parsed without 35 tag and till tag 777
    MsgReceived msg_data{"1=222\x1" "4=Y\x1" "7=223.456\x1" "13=Test string value\x1" "16=20221124\x1" "19=20221124 12:45:56\x1" "22=13\x1" "23=\x1\x2raw data \x1\x2\x1" 
        "28=2\x1" "29=1\x1" "4=Y\x1" "7=223.456\x1" "13=Test string value111\x1" "16=20221122\x1" "19=20221122 12:45:56\x1" "22=17\x1" "23=\x1\x2raw 111 data \x1\x2\x1" 
                  "29=2\x1" "4=N\x1" "7=113.456\x1" "13=Test string value222\x1" "16=20221111\x1" "19=20221111 12:45:56\x1" "22=17\x1" "23=\x1\x2raw 222 data \x1\x2\x1" 
        "2=2222\x1" "5=N\x1" "11=2223.456\x1" 
        "30=1\x1" "31=1122\x1" "1=1000\x1" "4=Y\x1" "7=223.456\x1" "13=Test string value111\x1" "16=20221122\x1" "19=20221122 12:45:56\x1" "22=17\x1" "23=\x1\x2raw 111 data \x1\x2\x1" "2=2000\x1" "5=N\x1" 
        "14=Test string value2\x1" "17=20221124\x1" "20=20221124 12:45:56\x1" "24=16\x1" "25=\x1\x2raw data222 \x1\x2\x1" 
        "32=1\x1" "33=1122\x1" "28=1\x1" "29=1011\x1" "777=111\x1", 0, "", 0};

    fix::FIXMessage test_msg = meta_msg.parse(msg_data);
    EXPECT_EQ("", msg_data.error_);

    {// check tags from 1st block
        EXPECT_EQ(222, test_msg.get_int(fixTest_gen::IntTag1));
        EXPECT_TRUE(test_msg.get_bool(fixTest_gen::BoolTag1));
        EXPECT_EQ("223.456", test_msg.get_double(fixTest_gen::DoubleTag1).to_string());
        EXPECT_EQ("Test string value", test_msg.get_string(fixTest_gen::StringTag1).to_string());
        EXPECT_EQ("20221124", test_msg.get_date(fixTest_gen::DateTag1).to_string());
        EXPECT_EQ("20221124 12:45:56", test_msg.get_datetime(fixTest_gen::DatetimeTag1).to_string());
        EXPECT_EQ(13, test_msg.get_int(fixTest_gen::RawLenTag1));
        EXPECT_EQ("\001\002raw data \001\002", test_msg.get_rawdata(fixTest_gen::RawDataTag1).to_string());
    }
    {// check tags from 2st block
        const auto &grp1 = test_msg.get_group(fixTest_gen::GrpSizeTag1);
        EXPECT_EQ(2, grp1.size());
        {
            const auto &entry1 = grp1.entry(0);
            EXPECT_EQ(1, entry1.get_int(fixTest_gen::GrpEntryTag1));
            EXPECT_TRUE(entry1.get_bool(fixTest_gen::BoolTag1));
            EXPECT_EQ("223.456", entry1.get_double(fixTest_gen::DoubleTag1).to_string());
            EXPECT_EQ("Test string value111", entry1.get_string(fixTest_gen::StringTag1).to_string());
            EXPECT_EQ("20221122", entry1.get_date(fixTest_gen::DateTag1).to_string());
            EXPECT_EQ("20221122 12:45:56", entry1.get_datetime(fixTest_gen::DatetimeTag1).to_string());
            EXPECT_EQ(17, entry1.get_int(fixTest_gen::RawLenTag1));
            EXPECT_EQ("\001\002raw 111 data \001\002", entry1.get_rawdata(fixTest_gen::RawDataTag1).to_string());

            const auto &entry2 = grp1.entry(1);
            EXPECT_EQ(2, entry2.get_int(fixTest_gen::GrpEntryTag1));
            EXPECT_FALSE(entry2.get_bool(fixTest_gen::BoolTag1));
            EXPECT_EQ("113.456", entry2.get_double(fixTest_gen::DoubleTag1).to_string());
            EXPECT_EQ("Test string value222", entry2.get_string(fixTest_gen::StringTag1).to_string());
            EXPECT_EQ("20221111", entry2.get_date(fixTest_gen::DateTag1).to_string());
            EXPECT_EQ("20221111 12:45:56", entry2.get_datetime(fixTest_gen::DatetimeTag1).to_string());
            EXPECT_EQ(17, entry2.get_int(fixTest_gen::RawLenTag1));
            EXPECT_EQ("\001\002raw 222 data \001\002", entry2.get_rawdata(fixTest_gen::RawDataTag1).to_string());
        }
        EXPECT_EQ(2222, test_msg.get_int(fixTest_gen::IntTag2));
        EXPECT_FALSE(test_msg.get_bool(fixTest_gen::BoolTag2));
        EXPECT_EQ("2223.456", test_msg.get_double(fixTest_gen::DoubleTag2).to_string());

        const auto &grp2 = test_msg.get_group(fixTest_gen::GrpSizeTag2);
        EXPECT_EQ(1, grp2.size());
        {
            const auto &entry1 = grp2.entry(0);
            EXPECT_EQ(1122, entry1.get_int(fixTest_gen::GrpEntryTag2));
            EXPECT_EQ(1000, entry1.get_int(fixTest_gen::IntTag1));
            EXPECT_TRUE(entry1.get_bool(fixTest_gen::BoolTag1));
            EXPECT_EQ("223.456", entry1.get_double(fixTest_gen::DoubleTag1).to_string());
            EXPECT_EQ("Test string value111", entry1.get_string(fixTest_gen::StringTag1).to_string());
            EXPECT_EQ("20221122", entry1.get_date(fixTest_gen::DateTag1).to_string());
            EXPECT_EQ("20221122 12:45:56", entry1.get_datetime(fixTest_gen::DatetimeTag1).to_string());
            EXPECT_EQ(17, entry1.get_int(fixTest_gen::RawLenTag1));
            EXPECT_EQ("\001\002raw 111 data \001\002", entry1.get_rawdata(fixTest_gen::RawDataTag1).to_string());

            EXPECT_EQ(2000, entry1.get_int(fixTest_gen::IntTag2));
            EXPECT_FALSE(entry1.get_bool(fixTest_gen::BoolTag2));
        }

        EXPECT_EQ("Test string value2", test_msg.get_string(fixTest_gen::StringTag2).to_string());
        EXPECT_EQ("20221124 12:45:56", test_msg.get_datetime(fixTest_gen::DatetimeTag2).to_string());
        EXPECT_EQ(16, test_msg.get_int(fixTest_gen::RawLenTag2));
        EXPECT_EQ("\001\002raw data222 \001\002", test_msg.get_rawdata(fixTest_gen::RawDataTag2).to_string());
        
        const auto &grp3 = test_msg.get_group(fixTest_gen::GrpSizeTag3);
        EXPECT_EQ(1, grp3.size());
        {
            const auto &entry1 = grp3.entry(0);
            EXPECT_EQ(1122, entry1.get_int(fixTest_gen::GrpEntryTag3));
            const auto &sub_grp1 = entry1.get_group(fixTest_gen::GrpSizeTag1);
            EXPECT_EQ(1, sub_grp1.size());
            const auto &entry11 = sub_grp1.entry(0);
            EXPECT_EQ(1011, entry11.get_int(fixTest_gen::GrpEntryTag1));
        }
    }
}

TEST(TestFIXMessage, parse_UT3_message){
    FIXDictionary dict_fixTest = FIXTestBuilder::build();
    fix::FIXMessageMetadata meta_msg("UT3", dict_fixTest);
    // message is parsed without 35 tag and till tag 777
    MsgReceived msg_data{"37=3\x1" "38=110011\x1" "38=110022\x1" "38=110033\x1" 
        "34=1\x1" "36=220011\x1" "32=1\x1" "33=230011\x1" 
            "28=2\x1" "29=240011\x1" "7=1100.24\x1" "13=Str Grp1->Grp3->Grp4, entry1\x1" 
                      "29=240012\x1" "7=1200.24\x1" "13=Str Grp1->Grp3->Grp4, entry2\x1" 
            "30=2\x1" "31=250011\x1" "1=2500111\x1" "2=2500112\x1" "5=Y\x1" 
                      "31=250012\x1" "1=2500121\x1" "2=2500122\x1" "5=N\x1" "777=111\x1", 0, "", 0};

    fix::FIXMessage test_msg = meta_msg.parse(msg_data);
    EXPECT_EQ("", msg_data.error_);

    {// fill tags for GrpSizeTag5 group
        const auto &grp1 = test_msg.get_group(fixTest_gen::GrpSizeTag5);
        EXPECT_EQ(3, grp1.size());
        {
            const auto &entry1 = grp1.entry(0);
            EXPECT_EQ(110011, entry1.get_int(fixTest_gen::GrpEntryTag5));
            const auto &entry2 = grp1.entry(1);
            EXPECT_EQ(110022, entry2.get_int(fixTest_gen::GrpEntryTag5));
            const auto &entry3 = grp1.entry(2);
            EXPECT_EQ(110033, entry3.get_int(fixTest_gen::GrpEntryTag5));
        }
    }
    {// fill tags for GrpSizeTag4 group
        const auto &grp1 = test_msg.get_group(fixTest_gen::GrpSizeTag4);
        EXPECT_EQ(1, grp1.size());
        {
            const auto &entry1 = grp1.entry(0);
            EXPECT_EQ(220011, entry1.get_int(fixTest_gen::GrpEntryTag4));

            const auto &subgrp1 = entry1.get_group(fixTest_gen::GrpSizeTag3);
            EXPECT_EQ(1, subgrp1.size());
            { //add sub group GrpSizeTag3
                const auto &entry1 = subgrp1.entry(0);
                EXPECT_EQ(230011, entry1.get_int(fixTest_gen::GrpEntryTag3));
                const auto &sub2grp1 = entry1.get_group(fixTest_gen::GrpSizeTag1);
                EXPECT_EQ(2, sub2grp1.size());
                { //add sub group GrpSizeTag3
                    const auto &entry1 = sub2grp1.entry(0);
                    EXPECT_EQ(240011, entry1.get_int(fixTest_gen::GrpEntryTag1));
                    EXPECT_EQ("1100.24", entry1.get_double(fixTest_gen::DoubleTag1).to_string());
                    EXPECT_EQ("Str Grp1->Grp3->Grp4, entry1", entry1.get_string(fixTest_gen::StringTag1).to_string());
                    const auto &entry2 = sub2grp1.entry(1);
                    EXPECT_EQ(240012, entry2.get_int(fixTest_gen::GrpEntryTag1));
                    EXPECT_EQ("1200.24", entry2.get_double(fixTest_gen::DoubleTag1).to_string());
                    EXPECT_EQ("Str Grp1->Grp3->Grp4, entry2", entry2.get_string(fixTest_gen::StringTag1).to_string());
                }
                auto &sub2grp2 = entry1.get_group(fixTest_gen::GrpSizeTag2);
                EXPECT_EQ(2, sub2grp2.size());
                { //add sub group GrpSizeTag2
                    auto &entry1 = sub2grp2.entry(0);
                    EXPECT_EQ(250011, entry1.get_int(fixTest_gen::GrpEntryTag2));
                    EXPECT_EQ(2500111, entry1.get_int(fixTest_gen::IntTag1));
                    EXPECT_EQ(2500112, entry1.get_int(fixTest_gen::IntTag2));
                    EXPECT_TRUE(entry1.get_bool(fixTest_gen::BoolTag2));
                    
                    auto &entry2 = sub2grp2.entry(1);
                    EXPECT_EQ(250012, entry2.get_int(fixTest_gen::GrpEntryTag2));
                    EXPECT_EQ(2500121, entry2.get_int(fixTest_gen::IntTag1));
                    EXPECT_EQ(2500122, entry2.get_int(fixTest_gen::IntTag2));
                    EXPECT_FALSE(entry2.get_bool(fixTest_gen::BoolTag2));
                }
            }
        }
    }
    
}
