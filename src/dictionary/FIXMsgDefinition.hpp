#pragma once

#include "../FIXParserDefines.hpp"
#include "FIXBlockDefinition.hpp"

#include <unordered_map>

namespace fix{
    // Contains definition of the FIX message - structure, tags, groups, etc
    class FIXMsgDefinition{
    public:
        FIXMsgDefinition(FIXProtocol protocol, const std::string &name, location_id_t location_id, 
                         const SupportedTagsT &tags, const SupportedBlocksT &blocks, 
                         const SupportedGroupsT &groups, const PositionsT &positions): 
            name_(name), protocol_(protocol), tags_(tags), blocks_(blocks), groups_(groups), positions_(positions), location_id_(location_id)
        {}

        FIXProtocol protocol()const noexcept{return protocol_;}
        const std::string &name()const noexcept{return name_;}
        location_id_t location_id()const noexcept{return location_id_;}

        const SupportedTagsT &tags()const noexcept{return tags_;};
        const SupportedBlocksT &blocks()const noexcept{return blocks_;};
        const SupportedGroupsT &groups()const noexcept{return groups_;};
        const PositionsT &positions()const noexcept{return positions_;};
        
        bool support_tag(tag_id_t tag_id)const noexcept;

    private:
        std::string name_;
        FIXProtocol protocol_;
        SupportedTagsT tags_;
        SupportedBlocksT blocks_;
        SupportedGroupsT groups_;
        PositionsT positions_;
        location_id_t location_id_;
    };

    using FIXMsgDefinitionsT = std::unordered_map<std::string, FIXMsgDefinition>;
}