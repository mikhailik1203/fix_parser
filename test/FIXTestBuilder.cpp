#include "FIXTestBuilder.hpp"
#include "../src/dictionary/FIXTagVocabulary.hpp"
#include "../src/dictionary/FIXBlockDefinition.hpp"
#include "../src/dictionary/FIXGroupDefinition.hpp"

using namespace fix;

namespace fixTest_gen{
    //---------------------------------------- Blocks ----------------------------------------------
    std::pair<FIXTestLocation, FIXBlockDefinition> build_test_block1(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIXTestLocation::TestBlock1, TagLocation::block};
        SupportedTagsT tags = {IntTag1, BoolTag1, DoubleTag1, StringTag1, DateTag1, DatetimeTag1, RawLenTag1, RawDataTag1};
        SupportedGroupsT groups;
        SupportedBlocksT blocks;
        PositionsT positions{{PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, {PositionType::tag, 3},
                             {PositionType::tag, 4}, {PositionType::tag, 5}, {PositionType::tag, 6}, {PositionType::tag, 7}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIXTestLocation::TestBlock1, FIXBlockDefinition(FIXTestLocation::TestBlock1, tags, blocks, groups, positions)};
    }

    std::pair<FIXTestLocation, FIXBlockDefinition> build_test_block2(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIXTestLocation::TestBlock2, TagLocation::block};
        SupportedGroupsT groups = {GrpSizeTag1, GrpSizeTag2, GrpSizeTag3};
        SupportedBlocksT blocks;
        SupportedTagsT tags = {IntTag2, BoolTag2, DoubleTag2, StringTag2, DateTag2, DatetimeTag2, RawLenTag2, RawDataTag2};
        PositionsT positions{{PositionType::group, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, {PositionType::group, 1},
                             {PositionType::tag, 3}, {PositionType::tag, 4}, {PositionType::tag, 5}, {PositionType::tag, 6}, {PositionType::tag, 7}, 
                             {PositionType::group, 2}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIXTestLocation::TestBlock2, FIXBlockDefinition(FIXTestLocation::TestBlock2, tags, blocks, groups, positions)};
    }

    std::pair<FIXTestLocation, FIXBlockDefinition> build_test_block3(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIXTestLocation::TestBlock3, TagLocation::block};
        SupportedBlocksT blocks = {TestBlock1, TestBlock2};
        SupportedGroupsT groups;
        SupportedTagsT tags = {IntTag3, StringTag3, RawLenTag3, RawDataTag3};
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, 
                            {PositionType::tag, 3}, {PositionType::block, 1}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIXTestLocation::TestBlock3, FIXBlockDefinition(FIXTestLocation::TestBlock3, tags, blocks, groups, positions)};
    }

    std::pair<FIXTestLocation, FIXBlockDefinition> build_test_block4(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIXTestLocation::TestBlock4, TagLocation::block};
        SupportedTagsT tags = {IntTag2, BoolTag2};
        SupportedGroupsT groups;
        SupportedBlocksT blocks;
        PositionsT positions{{PositionType::tag, 0}, {PositionType::tag, 1}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIXTestLocation::TestBlock4, FIXBlockDefinition(FIXTestLocation::TestBlock4, tags, blocks, groups, positions)};
    }

    std::pair<FIXTestLocation, FIXBlockDefinition> build_test_block5(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIXTestLocation::TestBlock5, TagLocation::block};
        SupportedTagsT tags = {IntTag3, BoolTag3};
        SupportedGroupsT groups;
        SupportedBlocksT blocks;
        PositionsT positions{{PositionType::tag, 0}, {PositionType::tag, 1}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIXTestLocation::TestBlock5, FIXBlockDefinition(FIXTestLocation::TestBlock5, tags, blocks, groups, positions)};
    }

    //---------------------------------------- Groups ----------------------------------------------
    std::pair<tag_id_t, FIXGroupDefinition> build_test_group1(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestGrp1, TagLocation::group};
        SupportedBlocksT blocks;
        SupportedGroupsT groups;
        SupportedTagsT tags = {GrpEntryTag1, BoolTag1, DoubleTag1, StringTag1, DateTag1, DatetimeTag1, RawLenTag1, RawDataTag1};
        PositionsT positions{{PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, {PositionType::tag, 3},
                             {PositionType::tag, 4}, {PositionType::tag, 5}, {PositionType::tag, 6}, {PositionType::tag, 7}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {GrpSizeTag1, FIXGroupDefinition(GrpSizeTag1, GrpEntryTag1, FIXTestLocation::TestGrp1, tags, blocks, groups, positions)};
    }

    std::pair<tag_id_t, FIXGroupDefinition> build_test_group2(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestGrp2, TagLocation::group};

        SupportedBlocksT blocks = {FIXTestLocation::TestBlock1, FIXTestLocation::TestBlock4};
        block_defs.find(FIXTestLocation::TestBlock1)->second.add_location(loc);
        block_defs.find(FIXTestLocation::TestBlock4)->second.add_location(loc);
        SupportedGroupsT groups;
        SupportedTagsT tags = {GrpEntryTag2};
        vocab.add_tag_location(GrpEntryTag2, loc);
        PositionsT positions{{PositionType::tag, 0}, {PositionType::block, 0}, {PositionType::block, 1}};

        return {GrpSizeTag2, FIXGroupDefinition(GrpSizeTag2, GrpEntryTag2, FIXTestLocation::TestGrp2, tags, blocks, groups, positions)};
    }

    std::pair<tag_id_t, FIXGroupDefinition> build_test_group3(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestGrp3, TagLocation::group};

        SupportedBlocksT blocks;
        SupportedGroupsT groups = {GrpSizeTag1, GrpSizeTag2};
        group_defs.find(GrpSizeTag1)->second.add_location(loc);
        group_defs.find(GrpSizeTag2)->second.add_location(loc);
        SupportedTagsT tags = {GrpEntryTag3};
        vocab.add_tag_location(GrpEntryTag3, loc);
        PositionsT positions{{PositionType::tag, 0}, {PositionType::group, 0}, {PositionType::group, 1}};
        return {GrpSizeTag3, FIXGroupDefinition(GrpSizeTag3, GrpEntryTag3, FIXTestLocation::TestGrp3, tags, blocks, groups, positions)};
    }

    std::pair<tag_id_t, FIXGroupDefinition> build_test_group4(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestGrp4, TagLocation::group};

        SupportedBlocksT blocks;
        SupportedGroupsT groups = {GrpSizeTag3};
        group_defs.find(GrpSizeTag3)->second.add_location(loc);
        SupportedTagsT tags = {GrpEntryTag4};
        vocab.add_tag_location(GrpEntryTag4, loc);
        PositionsT positions{{PositionType::tag, 0}, {PositionType::group, 0}};
        return {GrpSizeTag4, FIXGroupDefinition(GrpSizeTag4, GrpEntryTag4, FIXTestLocation::TestGrp4, tags, blocks, groups, positions)};
    }

    std::pair<tag_id_t, FIXGroupDefinition> build_test_group5(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestGrp5, TagLocation::group};

        SupportedBlocksT blocks;
        SupportedGroupsT groups;
        SupportedTagsT tags = {GrpEntryTag5};
        vocab.add_tag_location(GrpEntryTag5, loc);
        PositionsT positions{{PositionType::tag, 0}};
        return {GrpSizeTag5, FIXGroupDefinition(GrpSizeTag5, GrpEntryTag5, FIXTestLocation::TestGrp5, tags, blocks, groups, positions)};
    }

    //---------------------------------------- Messages ----------------------------------------------
    std::pair<std::string, FIXMsgDefinition> build_test_message1(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestMsg1, TagLocation::message};

        SupportedBlocksT blocks;
        SupportedGroupsT groups;
        SupportedTagsT tags = {IntTag1, BoolTag1, DoubleTag1, StringTag1, DateTag1, DatetimeTag1, RawLenTag1, RawDataTag1};
        PositionsT positions{{PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, {PositionType::tag, 3},
                             {PositionType::tag, 4}, {PositionType::tag, 5}, {PositionType::tag, 6}, {PositionType::tag, 7}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {"UT1", FIXMsgDefinition(FIXProtocol::FIXTest, "UT1", FIXTestLocation::TestMsg1, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_test_message2(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestMsg2, TagLocation::message};

        SupportedBlocksT blocks = {FIXTestLocation::TestBlock1, FIXTestLocation::TestBlock2};
        block_defs.find(FIXTestLocation::TestBlock1)->second.add_location(loc);
        block_defs.find(FIXTestLocation::TestBlock2)->second.add_location(loc);
        SupportedGroupsT groups;
        SupportedTagsT tags;
        PositionsT positions{{PositionType::block, 0}, {PositionType::block, 1}};

        return {"UT2", FIXMsgDefinition(FIXProtocol::FIXTest, "UT2", FIXTestLocation::TestMsg2, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_test_message3(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIXTestLocation::TestMsg3, TagLocation::message};

        SupportedBlocksT blocks;
        SupportedGroupsT groups = {GrpSizeTag5, GrpSizeTag4};
        group_defs.find(GrpSizeTag5)->second.add_location(loc);
        group_defs.find(GrpSizeTag4)->second.add_location(loc);
        SupportedTagsT tags;
        PositionsT positions{{PositionType::group, 0}, {PositionType::group, 1}};

        return {"UT3", FIXMsgDefinition(FIXProtocol::FIXTest, "UT3", FIXTestLocation::TestMsg3, tags, blocks, groups, positions)};
    }
}

using namespace fixTest_gen;

FIXDictionary FIXTestBuilder::build(){
    auto vocab = build_vocabulary();
    auto blocks = build_blocks(vocab);
    auto groups = build_groups(vocab, blocks);
    auto messages = build_messages(vocab, blocks, groups);
    return FIXDictionary(FIXProtocol::FIXTest, vocab, blocks, groups, messages);
}

FIXTagVocabulary FIXTestBuilder::build_vocabulary(){
    TagDefinitionsT tags{{IntTag1, FIXTagDefinition(IntTag1, FIXTagType::INT)}, 
                         {IntTag2, FIXTagDefinition(IntTag2, FIXTagType::INT)},
                         {IntTag3, FIXTagDefinition(IntTag3, FIXTagType::INT)},
                         {BoolTag1, FIXTagDefinition(BoolTag1, FIXTagType::BOOL)},
                         {BoolTag2, FIXTagDefinition(BoolTag2, FIXTagType::BOOL)},
                         {BoolTag3, FIXTagDefinition(BoolTag3, FIXTagType::BOOL)},
                         {DoubleTag1, FIXTagDefinition(DoubleTag1, FIXTagType::DOUBLE)},
                         {DoubleTag2, FIXTagDefinition(DoubleTag2, FIXTagType::DOUBLE)},
                         {DoubleTag3, FIXTagDefinition(DoubleTag3, FIXTagType::DOUBLE)},
                         {StringTag1, FIXTagDefinition(StringTag1, FIXTagType::STRING)},
                         {StringTag2, FIXTagDefinition(StringTag2, FIXTagType::STRING)},
                         {StringTag3, FIXTagDefinition(StringTag3, FIXTagType::STRING)},
                         {DateTag1, FIXTagDefinition(DateTag1, FIXTagType::DATE)},
                         {DateTag2, FIXTagDefinition(DateTag2, FIXTagType::DATE)},
                         {DateTag3, FIXTagDefinition(DateTag3, FIXTagType::DATE)},
                         {DatetimeTag1, FIXTagDefinition(DatetimeTag1, FIXTagType::DATETIME)},
                         {DatetimeTag2, FIXTagDefinition(DatetimeTag2, FIXTagType::DATETIME)},
                         {DatetimeTag3, FIXTagDefinition(DatetimeTag3, FIXTagType::DATETIME)},
                         {RawLenTag1, FIXTagDefinition(RawLenTag1, FIXTagType::RAWDATALEN)},
                         {RawLenTag2, FIXTagDefinition(RawLenTag2, FIXTagType::RAWDATALEN)},
                         {RawLenTag3, FIXTagDefinition(RawLenTag3, FIXTagType::RAWDATALEN)},
                         {RawDataTag1, FIXTagDefinition(RawDataTag1, FIXTagType::RAWDATA)},
                         {RawDataTag2, FIXTagDefinition(RawDataTag2, FIXTagType::RAWDATA)},
                         {RawDataTag3, FIXTagDefinition(RawDataTag3, FIXTagType::RAWDATA)},
                         {GrpSizeTag1, FIXTagDefinition(GrpSizeTag1, FIXTagType::INT)},
                         {GrpSizeTag2, FIXTagDefinition(GrpSizeTag2, FIXTagType::INT)},
                         {GrpSizeTag3, FIXTagDefinition(GrpSizeTag3, FIXTagType::INT)},
                         {GrpSizeTag4, FIXTagDefinition(GrpSizeTag4, FIXTagType::INT)},
                         {GrpSizeTag5, FIXTagDefinition(GrpSizeTag5, FIXTagType::INT)},
                         {GrpEntryTag1, FIXTagDefinition(GrpEntryTag1, FIXTagType::INT)},
                         {GrpEntryTag2, FIXTagDefinition(GrpEntryTag2, FIXTagType::INT)},
                         {GrpEntryTag3, FIXTagDefinition(GrpEntryTag3, FIXTagType::INT)},
                         {GrpEntryTag4, FIXTagDefinition(GrpEntryTag4, FIXTagType::INT)},
                         {GrpEntryTag5, FIXTagDefinition(GrpEntryTag5, FIXTagType::INT)},
                        };

    return FIXTagVocabulary(FIXProtocol::FIXTest, tags);
}


FIXBlockDefinitionsT FIXTestBuilder::build_blocks(FIXTagVocabulary &vocab){
    return FIXBlockDefinitionsT{build_test_block1(vocab), 
                                build_test_block2(vocab),
                                build_test_block3(vocab),
                                build_test_block4(vocab),
                                build_test_block5(vocab)};
}

FIXGroupDefinitionsT FIXTestBuilder::build_groups(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs){
    FIXGroupDefinitionsT grp_defs;
    grp_defs.insert(build_test_group1(vocab, block_defs, grp_defs));
    grp_defs.insert(build_test_group2(vocab, block_defs, grp_defs));
    grp_defs.insert(build_test_group3(vocab, block_defs, grp_defs));
    grp_defs.insert(build_test_group4(vocab, block_defs, grp_defs));
    grp_defs.insert(build_test_group5(vocab, block_defs, grp_defs));
    return grp_defs;
}

FIXMsgDefinitionsT FIXTestBuilder::build_messages(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
    return FIXMsgDefinitionsT{build_test_message1(vocab, block_defs, group_defs), 
                            build_test_message2(vocab, block_defs, group_defs), 
                            build_test_message3(vocab, block_defs, group_defs)};
}