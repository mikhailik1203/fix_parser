#pragma once

#include "../FIXParserDefines.hpp"
#include "FIXTagDefinition.hpp"

#include <unordered_map>

namespace fix{
    // Contains definition of the FIX block - structure, tags, subgroups, etc
    class FIXBlockDefinition{
    public:
        FIXBlockDefinition(location_id_t block_id, const SupportedTagsT &tags, const SupportedBlocksT &blocks, const SupportedGroupsT &groups, const PositionsT &positions): 
                block_id_(block_id), tags_(tags), blocks_(blocks), groups_(groups), positions_(positions){}

        location_id_t block_id()const noexcept{return block_id_;}

        const SupportedTagsT &tags()const noexcept{return tags_;};
        const SupportedBlocksT &blocks()const noexcept{return blocks_;};
        const SupportedGroupsT &groups()const noexcept{return groups_;};
        const PositionsT &positions()const noexcept{return positions_;};

        // return true, in case tag is defined in the group
        bool support_tag(tag_id_t tag_id)const noexcept;

        const TagLocationsT &locations()const noexcept{return block_locations_;}

        void add_location(const FIXTagLocation &val);
    private:
        SupportedTagsT tags_;
        SupportedBlocksT blocks_;
        SupportedGroupsT groups_;
        TagLocationsT block_locations_; // messages and groups, where this block is located
        PositionsT positions_;
        location_id_t block_id_ = 0;
    };

    using FIXBlockDefinitionsT = std::unordered_map<location_id_t, FIXBlockDefinition>;
}