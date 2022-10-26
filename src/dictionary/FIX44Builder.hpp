#pragma once

#include "../FIXParserDefines.hpp"
#include "FIXDictionary.hpp"


namespace fix44_gen{
    enum FIX44Tags{
        Account = 1,
        BeginSeqNo = 7, 
        ClOrdID = 11,
        EndSeqNo =16,
        ExecInst = 18,
        MsgSeqNum = 34, 
        MsgType = 35,
        NewSeqNo = 36,
        OrderQty = 38,
        OrdType = 40,
        Price = 44,
        RefSeqNum = 45,
        SenderCompID = 49,
        SendingTime = 52,
        Side = 54,
        Symbol = 55,
        TargetCompID = 56,
        Text = 58,
        TimeInForce = 59,
        RawDataLength = 95,
        RawData = 96,
        HeartBtInt = 108,
        TestReqId = 112,
        NoRelatedSym = 146,
        MdReqId = 262,
        SubscriptionRequestType = 263, 
        MarketDepth = 264,
        MDUpdateType = 265,
        NoMdEntryTypes = 267,
        MDEntryType = 269,
        RefMsgType = 372,
        SessionRejectReason = 373,
        Username = 553,
        Password = 554,
    };

    enum FIX44Location{
        invalid = 0,
        // block range
        HeaderBlock,
        InstrumentBlock,

        // group range
        MDEntryTypesGroup,
        InstrumentsGroup,

        // message range
        LogonMessage,
        LogoutMessage,
        HeartBeatMessage,
        TestRequestMessage,
        ResendRequestMessage,
        RejectMessage,
        SequenceResetMessage,

        MarketDataRequestMessage,
        NewOrderSingleMessage,
    };

}

namespace fix{
    class FIXTagVocabulary;
    class FIXDictionary;

    // Builds vocabulary and messages&groups&blocks definitions for FIX44 
    class FIX44Builder{
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