#pragma once

#include "../FIXParserDefines.hpp"
#include "FIXTagDefinition.hpp"

#include <unordered_map>

namespace fix{
    // Contains definition of the FIX group - structure, tags, subgroups, etc
    class FIXGroupDefinition{
    public:
        FIXGroupDefinition(tag_id_t leading_tag_id, tag_id_t entry_start_tag_id, 
                            location_id_t location_id, const SupportedTagsT &tags, 
                            const SupportedBlocksT &blocks, const SupportedGroupsT &groups,
                            const PositionsT &positions): 
            tags_(tags), blocks_(blocks), groups_(groups), leading_tag_id_(leading_tag_id), 
            entry_start_tag_id_(entry_start_tag_id), location_id_(location_id), positions_(positions)
        {}

        // returns leading tag of the group
        tag_id_t leading_tag()const noexcept{return leading_tag_id_;}
        tag_id_t entry_tag()const noexcept{return entry_start_tag_id_;}
        location_id_t location_id()const noexcept{return location_id_;}

        // return true, in case tag is drfined in the group
        bool support_tag(tag_id_t tag_id)const noexcept;

        const TagLocationsT &locations()const noexcept{return group_locations_;}

        void add_location(const FIXTagLocation &val);

        const SupportedTagsT &tags()const noexcept{return tags_;};
        const SupportedBlocksT &blocks()const noexcept{return blocks_;};
        const SupportedGroupsT &groups()const noexcept{return groups_;};
        const PositionsT &positions()const noexcept{return positions_;};

    private:
        SupportedTagsT tags_;
        SupportedBlocksT blocks_;
        SupportedGroupsT groups_;
        TagLocationsT group_locations_; // messages, blocks and groups, where this group is used/located
        PositionsT positions_;
        tag_id_t leading_tag_id_ = 0;
        tag_id_t entry_start_tag_id_ = 0;
        location_id_t location_id_ = 0;
    };

    using FIXGroupDefinitionsT = std::unordered_map<tag_id_t, FIXGroupDefinition>;
}