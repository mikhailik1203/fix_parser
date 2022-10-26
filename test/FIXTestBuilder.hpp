#pragma once

#include "../src/FIXParserDefines.hpp"
#include "../src/dictionary/FIXDictionary.hpp"


namespace fixTest_gen{
    enum FIXTestTags{
        IntTag1 = 1,
        IntTag2, 
        IntTag3,
        BoolTag1,
        BoolTag2,
        BoolTag3,
        DoubleTag1,
        Protocol, //=8 to make it compatible to FIX
        MsgBodyLen, //=9 to make it compatible to FIX
        CRCSum, //=10 to make it compatible to FIX
        DoubleTag2,
        DoubleTag3,
        StringTag1,
        StringTag2,
        StringTag3,
        DateTag1,
        DateTag2,
        DateTag3,
        DatetimeTag1,
        DatetimeTag2,
        DatetimeTag3,
        RawLenTag1,
        RawDataTag1,
        RawLenTag2,
        RawDataTag2,
        RawLenTag3,
        RawDataTag3,
        GrpSizeTag1,
        GrpEntryTag1,
        GrpSizeTag2,
        GrpEntryTag2,
        GrpSizeTag3,
        GrpEntryTag3,
        GrpSizeTag4,
        MsgType = 35, // to make it compatible to FIX
        GrpEntryTag4,
        GrpSizeTag5,
        GrpEntryTag5,
    };

    enum FIXTestLocation{
        invalid = 0,
        // block range
        TestBlock1,
        TestBlock2,
        TestBlock3,
        TestBlock4,
        TestBlock5,

        // group range
        TestGrp1,
        TestGrp2,
        TestGrp3,
        TestGrp4,
        TestGrp5,

        // message range
        TestMsg1,
        TestMsg2,
        TestMsg3,
    };

}

namespace fix{
    class FIXTagVocabulary;
    class FIXDictionary;


    // Builds vocabulary and messages&groups&blocks definitions for FIX44 
    class FIXTestBuilder{
    public:
        static FIXDictionary build();

    protected:
        static FIXTagVocabulary build_vocabulary();
        static FIXBlockDefinitionsT build_blocks(FIXTagVocabulary &vocab);
        static FIXGroupDefinitionsT build_groups(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs);
        static FIXMsgDefinitionsT build_messages(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, 
                                        FIXGroupDefinitionsT &group_defs);
    };
}