#include "FIX44Builder.hpp"
#include "FIXTagVocabulary.hpp"
#include "FIXBlockDefinition.hpp"
#include "FIXGroupDefinition.hpp"

using namespace fix;

namespace fix44_gen{
    //---------------------------------------- Blocks ----------------------------------------------
    std::pair<FIX44Location, FIXBlockDefinition> build_header_block(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIX44Location::HeaderBlock, TagLocation::block};
        SupportedTagsT tags = {SenderCompID, TargetCompID, MsgSeqNum, SendingTime};
        SupportedGroupsT groups;
        SupportedBlocksT blocks;
        PositionsT positions{{PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, {PositionType::tag, 3}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIX44Location::HeaderBlock, FIXBlockDefinition(FIX44Location::HeaderBlock, tags, blocks, groups, positions)};
    }

    std::pair<FIX44Location, FIXBlockDefinition> build_instrument_block(FIXTagVocabulary &vocab){
        FIXTagLocation loc{FIX44Location::InstrumentBlock, TagLocation::block};
        SupportedTagsT tags = {Symbol};
        SupportedGroupsT groups;
        SupportedBlocksT blocks;
        PositionsT positions{{PositionType::tag, 0}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {FIX44Location::InstrumentBlock, FIXBlockDefinition(FIX44Location::InstrumentBlock, tags, blocks, groups, positions)};
    }
    
    //---------------------------------------- Groups ----------------------------------------------
    std::pair<tag_id_t, FIXGroupDefinition> build_mdentrytype_group(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs){
        FIXTagLocation loc{FIX44Location::MDEntryTypesGroup, TagLocation::group};
        SupportedBlocksT blocks;
        SupportedGroupsT groups;
        SupportedTagsT tags = {MDEntryType};
        PositionsT positions{{PositionType::tag, 0}};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }

        return {NoMdEntryTypes, FIXGroupDefinition(NoMdEntryTypes, MDEntryType, FIX44Location::MDEntryTypesGroup, tags, blocks, groups, positions)};
    }

    std::pair<tag_id_t, FIXGroupDefinition> build_instruments_group(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs){
        FIXTagLocation loc{FIX44Location::InstrumentsGroup, TagLocation::group};

        SupportedBlocksT blocks = {FIX44Location::InstrumentBlock};
        block_defs.find(FIX44Location::InstrumentBlock)->second.add_location(loc);
        SupportedGroupsT groups;
        PositionsT positions{{PositionType::block, 0}};

        SupportedTagsT tags;
        return {NoRelatedSym, FIXGroupDefinition(NoRelatedSym, Symbol, FIX44Location::InstrumentsGroup, tags, blocks, groups, positions)};
    }

    //---------------------------------------- Messages ----------------------------------------------
    std::pair<std::string, FIXMsgDefinition> build_logon_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::LogonMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {HeartBtInt, RawDataLength, RawData, Username, Password};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, 
                             {PositionType::tag, 3}, {PositionType::tag, 4}};

        return {"A", FIXMsgDefinition(FIXProtocol::FIX44, "A", FIX44Location::LogonMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_logout_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::LogoutMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {Text};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}};

        return {"5", FIXMsgDefinition(FIXProtocol::FIX44, "5", FIX44Location::LogoutMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_heartbeat_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::HeartBeatMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {TestReqId};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}};

        return {"0", FIXMsgDefinition(FIXProtocol::FIX44, "0", FIX44Location::HeartBeatMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_test_request_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::TestRequestMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {TestReqId};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}};

        return {"1", FIXMsgDefinition(FIXProtocol::FIX44, "1", FIX44Location::TestRequestMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_resend_request_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::ResendRequestMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {BeginSeqNo, EndSeqNo};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}};

        return {"2", FIXMsgDefinition(FIXProtocol::FIX44, "2", FIX44Location::ResendRequestMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_reject_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::RejectMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {RefSeqNum, RefMsgType, SessionRejectReason, Text};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}, {PositionType::tag, 2}, {PositionType::tag, 3}};

        return {"3", FIXMsgDefinition(FIXProtocol::FIX44, "3", FIX44Location::RejectMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_sequence_reset_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::SequenceResetMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);
        SupportedGroupsT groups;

        SupportedTagsT tags = {NewSeqNo};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}};

        return {"4", FIXMsgDefinition(FIXProtocol::FIX44, "4", FIX44Location::SequenceResetMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_marketdatarequest_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::MarketDataRequestMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);

        SupportedGroupsT groups = {NoMdEntryTypes, NoRelatedSym};
        for(auto grp_tag_id: groups){
            group_defs.find(grp_tag_id)->second.add_location(loc);
        }

        SupportedTagsT tags = {MdReqId, SubscriptionRequestType, MarketDepth, MDUpdateType, NoMdEntryTypes, NoRelatedSym};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}, 
                {PositionType::tag, 2}, {PositionType::tag, 3}, {PositionType::group, 0}, {PositionType::group, 1}};

        return {"V", FIXMsgDefinition(FIXProtocol::FIX44, "V", FIX44Location::MarketDataRequestMessage, tags, blocks, groups, positions)};
    }

    std::pair<std::string, FIXMsgDefinition> build_newordersingle_message(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
        FIXTagLocation loc{FIX44Location::NewOrderSingleMessage, TagLocation::message};

        SupportedBlocksT blocks = {FIX44Location::HeaderBlock};
        block_defs.find(FIX44Location::HeaderBlock)->second.add_location(loc);

        SupportedGroupsT groups;

        SupportedTagsT tags = {ClOrdID, Side, OrderQty, Price, Symbol, ExecInst, OrdType, TimeInForce};
        for(auto tag_id: tags){
            vocab.add_tag_location(tag_id, loc);
        }
        PositionsT positions{{PositionType::block, 0}, {PositionType::tag, 0}, {PositionType::tag, 1}, 
                {PositionType::tag, 2}, {PositionType::tag, 3}, {PositionType::tag, 4}, {PositionType::tag, 5}, 
                {PositionType::tag, 6}, {PositionType::tag, 7}};

        return {"D", FIXMsgDefinition(FIXProtocol::FIX44, "D", FIX44Location::NewOrderSingleMessage, tags, blocks, groups, positions)};
    }

}

using namespace fix44_gen;

FIXDictionary FIX44Builder::build(){
    auto vocab = build_vocabulary();
    auto blocks = build_blocks(vocab);
    auto groups = build_groups(vocab, blocks);
    auto messages = build_messages(vocab, blocks, groups);
    return FIXDictionary(FIXProtocol::FIX44, vocab, blocks, groups, messages);
}

FIXTagVocabulary FIX44Builder::build_vocabulary(){
    TagDefinitionsT tags{{Account, FIXTagDefinition(Account, FIXTagType::STRING)}, 
                         {BeginSeqNo, FIXTagDefinition(BeginSeqNo, FIXTagType::INT)},
                         {ClOrdID, FIXTagDefinition(ClOrdID, FIXTagType::STRING)}, 
                         {EndSeqNo, FIXTagDefinition(EndSeqNo, FIXTagType::INT)},
                         {ExecInst, FIXTagDefinition(ExecInst, FIXTagType::STRING)}, 
                         {MsgType, FIXTagDefinition(MsgType, FIXTagType::STRING)},
                         {NewSeqNo, FIXTagDefinition(NewSeqNo, FIXTagType::INT)},
                         {OrderQty, FIXTagDefinition(OrderQty, FIXTagType::DOUBLE)},
                         {OrdType, FIXTagDefinition(OrdType, FIXTagType::CHAR)},
                         {Price, FIXTagDefinition(Price, FIXTagType::DOUBLE)},
                         {RefSeqNum, FIXTagDefinition(RefSeqNum, FIXTagType::INT)},
                         {SenderCompID, FIXTagDefinition(SenderCompID, FIXTagType::STRING)},
                         {TargetCompID, FIXTagDefinition(TargetCompID, FIXTagType::STRING)},
                         {Text, FIXTagDefinition(Text, FIXTagType::STRING)},
                         {MsgSeqNum, FIXTagDefinition(MsgSeqNum, FIXTagType::INT)},
                         {SendingTime, FIXTagDefinition(SendingTime, FIXTagType::DATETIME)},
                         {Side, FIXTagDefinition(Side, FIXTagType::CHAR)},
                         {HeartBtInt, FIXTagDefinition(HeartBtInt, FIXTagType::INT)},
                         {TimeInForce, FIXTagDefinition(TimeInForce, FIXTagType::CHAR)},
                         {RawDataLength, FIXTagDefinition(RawDataLength, FIXTagType::RAWDATALEN)},
                         {RawData, FIXTagDefinition(RawData, FIXTagType::RAWDATA)},
                         {TestReqId, FIXTagDefinition(TestReqId, FIXTagType::STRING)},
                         {RefMsgType, FIXTagDefinition(RefMsgType, FIXTagType::STRING)},
                         {SessionRejectReason, FIXTagDefinition(SessionRejectReason, FIXTagType::INT)},
                         {Username, FIXTagDefinition(Username, FIXTagType::STRING)},
                         {Password, FIXTagDefinition(Password, FIXTagType::STRING)},
                         {MdReqId, FIXTagDefinition(MdReqId, FIXTagType::STRING)},
                         {SubscriptionRequestType, FIXTagDefinition(SubscriptionRequestType, FIXTagType::INT)},
                         {MarketDepth, FIXTagDefinition(MarketDepth, FIXTagType::INT)},
                         {MDUpdateType, FIXTagDefinition(MDUpdateType, FIXTagType::INT)},
                         {NoMdEntryTypes, FIXTagDefinition(NoMdEntryTypes, FIXTagType::INT)},
                         {MDEntryType, FIXTagDefinition(MDEntryType, FIXTagType::INT)},
                         {NoRelatedSym, FIXTagDefinition(NoRelatedSym, FIXTagType::INT)},
                         {Symbol, FIXTagDefinition(Symbol, FIXTagType::STRING)}
                         };

    return FIXTagVocabulary(FIXProtocol::FIX44, tags);
}


FIXBlockDefinitionsT FIX44Builder::build_blocks(FIXTagVocabulary &vocab){
    return FIXBlockDefinitionsT{build_header_block(vocab), 
                                build_instrument_block(vocab)};
}

FIXGroupDefinitionsT FIX44Builder::build_groups(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs){
    return FIXGroupDefinitionsT{build_mdentrytype_group(vocab, block_defs), 
                                build_instruments_group(vocab, block_defs)};
}

FIXMsgDefinitionsT FIX44Builder::build_messages(FIXTagVocabulary &vocab, FIXBlockDefinitionsT &block_defs, FIXGroupDefinitionsT &group_defs){
    return FIXMsgDefinitionsT{build_logon_message(vocab, block_defs, group_defs), 
                            build_logout_message(vocab, block_defs, group_defs), 
                            build_heartbeat_message(vocab, block_defs, group_defs), 
                            build_test_request_message(vocab, block_defs, group_defs),
                            build_resend_request_message(vocab, block_defs, group_defs), 
                            build_reject_message(vocab, block_defs, group_defs),
                            build_sequence_reset_message(vocab, block_defs, group_defs), 
                            build_marketdatarequest_message(vocab, block_defs, group_defs),
                            build_newordersingle_message(vocab, block_defs, group_defs)};
}