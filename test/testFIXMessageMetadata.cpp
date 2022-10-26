#include <gtest/gtest.h>
#include "../src/FIXMessageMetadata.hpp"
#include "../src/dictionary/FIX44Builder.hpp"

using namespace fix;

TEST(TestFIXMessageMetadata, create_marketdata_request){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXMessageMetadata test_val("V", dict_fix44);
    EXPECT_EQ(FIXProtocol::FIX44, test_val.protocol());
    EXPECT_EQ("V", test_val.name());
    EXPECT_EQ(fix44_gen::MarketDataRequestMessage, test_val.location_id());

    EXPECT_FALSE(test_val.support_tag(fix44_gen::MsgType));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::SenderCompID));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::TargetCompID));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::MsgSeqNum));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::SendingTime));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::MdReqId));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::SubscriptionRequestType));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::MarketDepth));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::MDUpdateType));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::NoMdEntryTypes));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::NoRelatedSym));
    EXPECT_FALSE(test_val.support_tag(fix44_gen::MDEntryType));
    EXPECT_FALSE(test_val.support_tag(fix44_gen::Symbol));

    EXPECT_EQ(FIXTagType::invalid, test_val.tag_type(fix44_gen::MDEntryType));
    EXPECT_EQ(FIXTagType::invalid, test_val.tag_type(fix44_gen::Symbol));
    EXPECT_EQ(FIXTagType::invalid, test_val.tag_type(fix44_gen::MsgType));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::SenderCompID));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::TargetCompID));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::MsgSeqNum));
    EXPECT_EQ(FIXTagType::DATETIME, test_val.tag_type(fix44_gen::SendingTime));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::MdReqId));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::SubscriptionRequestType));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::MarketDepth));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::MDUpdateType));
    EXPECT_EQ(FIXTagType::GROUP, test_val.tag_type(fix44_gen::NoMdEntryTypes));
    EXPECT_EQ(FIXTagType::GROUP, test_val.tag_type(fix44_gen::NoRelatedSym));

    EXPECT_EQ(4, test_val.tag_int_count());
    EXPECT_EQ(3, test_val.tag_string_count());
    EXPECT_EQ(1, test_val.tag_datetime_count());
    EXPECT_EQ(0, test_val.tag_rawdata_count());
    EXPECT_EQ(2, test_val.groups().size());
}

TEST(TestFIXMessageMetadata, create_logon){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXMessageMetadata test_val("A", dict_fix44);
    EXPECT_EQ(FIXProtocol::FIX44, test_val.protocol());
    EXPECT_EQ("A", test_val.name());
    EXPECT_EQ(fix44_gen::LogonMessage, test_val.location_id());

    EXPECT_FALSE(test_val.support_tag(fix44_gen::MsgType));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::SenderCompID));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::TargetCompID));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::MsgSeqNum));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::SendingTime));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::HeartBtInt));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::RawDataLength));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::RawData));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::Username));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::Password));

    EXPECT_EQ(FIXTagType::invalid, test_val.tag_type(fix44_gen::MsgType));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::SenderCompID));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::TargetCompID));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::MsgSeqNum));
    EXPECT_EQ(FIXTagType::DATETIME, test_val.tag_type(fix44_gen::SendingTime));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::HeartBtInt));
    EXPECT_EQ(FIXTagType::RAWDATALEN, test_val.tag_type(fix44_gen::RawDataLength));
    EXPECT_EQ(FIXTagType::RAWDATA, test_val.tag_type(fix44_gen::RawData));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::Username));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::Password));

    EXPECT_EQ(3, test_val.tag_int_count());
    EXPECT_EQ(4, test_val.tag_string_count());
    EXPECT_EQ(1, test_val.tag_datetime_count());
    EXPECT_EQ(1, test_val.tag_rawdata_count());
    EXPECT_EQ(0, test_val.groups().size());
}