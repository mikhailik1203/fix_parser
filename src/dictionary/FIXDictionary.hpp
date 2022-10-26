#pragma once

#include "../FIXParserDefines.hpp"
#include "FIXTagVocabulary.hpp"
#include "FIXBlockDefinition.hpp"
#include "FIXGroupDefinition.hpp"
#include "FIXMsgDefinition.hpp"

namespace fix{
    // Encapsulates definitions for all messages, defined for this protocol
    class FIXDictionary{
    public:
        FIXDictionary(FIXProtocol protocol, const FIXTagVocabulary &tags, const FIXBlockDefinitionsT &blocks, 
                      const FIXGroupDefinitionsT &groups, const FIXMsgDefinitionsT &messages):
            protocol_(protocol), tags_(tags), blocks_(blocks), groups_(groups), messages_(messages){}

        FIXProtocol protocol()const noexcept{return protocol_;}

        // returns tag's vocabulary
        const FIXTagVocabulary &vocabulary()const noexcept{return tags_;}
        const FIXBlockDefinitionsT &blocks()const noexcept{return blocks_;}
        const FIXGroupDefinitionsT &groups()const noexcept{return groups_;}
        const FIXMsgDefinitionsT &messages()const noexcept{return messages_;}

        // returns definition of the message
        const FIXMsgDefinition &find_message(const FIXMsgType &type)const noexcept;

    private:
        FIXProtocol protocol_;
        FIXTagVocabulary tags_;
        FIXBlockDefinitionsT blocks_;
        FIXGroupDefinitionsT groups_;
        FIXMsgDefinitionsT messages_;
    };
}