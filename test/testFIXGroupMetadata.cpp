#include <gtest/gtest.h>
#include "../src/FIXGroupMetadata.hpp"
#include "../src/dictionary/FIX44Builder.hpp"

using namespace fix;

TEST(TestFIXGroupMetadata, create_mdentrytypes){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata test_val(fix44_gen::NoMdEntryTypes, dict_fix44);
    EXPECT_EQ(fix44_gen::NoMdEntryTypes, test_val.leading_tag());
    EXPECT_EQ(fix44_gen::MDEntryType, test_val.entry_tag());
    EXPECT_EQ(fix44_gen::MDEntryTypesGroup, test_val.location_id());

    EXPECT_FALSE(test_val.support_tag(fix44_gen::NoMdEntryTypes));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::MDEntryType));

    EXPECT_EQ(FIXTagType::invalid, test_val.tag_type(fix44_gen::NoMdEntryTypes));
    EXPECT_EQ(FIXTagType::INT, test_val.tag_type(fix44_gen::MDEntryType));

    EXPECT_EQ(1, test_val.tag_int_count());
    EXPECT_EQ(0, test_val.tag_string_count());
    EXPECT_EQ(0, test_val.tag_datetime_count());
    EXPECT_EQ(0, test_val.tag_rawdata_count());
    EXPECT_EQ(0, test_val.groups().size());
}

TEST(TestFIXGroupMetadata, create_instruments){
    FIXDictionary dict_fix44 = FIX44Builder::build();
    fix::FIXGroupMetadata test_val(fix44_gen::NoRelatedSym, dict_fix44);
    EXPECT_EQ(fix44_gen::NoRelatedSym, test_val.leading_tag());
    EXPECT_EQ(fix44_gen::Symbol, test_val.entry_tag());
    EXPECT_EQ(fix44_gen::InstrumentsGroup, test_val.location_id());

    EXPECT_FALSE(test_val.support_tag(fix44_gen::NoRelatedSym));
    EXPECT_TRUE(test_val.support_tag(fix44_gen::Symbol));

    EXPECT_EQ(FIXTagType::invalid, test_val.tag_type(fix44_gen::NoRelatedSym));
    EXPECT_EQ(FIXTagType::STRING, test_val.tag_type(fix44_gen::Symbol));

    EXPECT_EQ(0, test_val.tag_int_count());
    EXPECT_EQ(1, test_val.tag_string_count());
    EXPECT_EQ(0, test_val.tag_datetime_count());
    EXPECT_EQ(0, test_val.tag_rawdata_count());
    EXPECT_EQ(0, test_val.groups().size());
}

