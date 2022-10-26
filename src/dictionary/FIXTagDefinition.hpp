#pragma once

#include "../FIXParserDefines.hpp"
#include <unordered_set>
#include <unordered_map>

namespace fix{

    enum class TagLocation{
        invalid = 0,
        message,
        group,
        block
    };
    struct FIXTagLocation{
        location_id_t location_id_ = 0; // id of the message, group, block, etc
        TagLocation location_type_ = TagLocation::invalid; // type of the location
    };
    struct TagLocHash{
        size_t operator()(const FIXTagLocation &val)const noexcept{
            return std::hash<location_id_t>()(val.location_id_) ^ std::hash<char>()(static_cast<char>(val.location_type_));
        }
    };
    struct TagLocEqual{
        size_t operator()(const FIXTagLocation &left, const FIXTagLocation &right)const noexcept{
            return left.location_id_ == right.location_id_ && left.location_type_ == right.location_type_;
        }
    };

    using TagLocationsT = std::unordered_set<FIXTagLocation, TagLocHash, TagLocEqual>;

    enum class PositionType: char{
        invalid = 0,
        tag,
        group,
        block
    };

    struct PositionData{
        PositionType type_ = PositionType::invalid;
        char index_ = 0; // index in the tag/block/group set
    };
    using PositionsT = std::vector<PositionData>;

    // Contains definition of the FIX tag - type, in which groups, blocks or messages it could be used, etc
    class FIXTagDefinition{
    public:
        FIXTagDefinition(tag_id_t tag_id, FIXTagType type): tag_id_(tag_id), type_(type){}
        
        // returns tag_id
        tag_id_t tag_id()const noexcept{return tag_id_;}

        // return type of the tag
        FIXTagType tag_type()const noexcept{return type_;}

        TagLocationsT locations()const noexcept{return locations_;}

        void add_location(const FIXTagLocation &val);

    private:
        TagLocationsT locations_; // messages, blocks and groups, where this tag is used/located
        tag_id_t tag_id_;
        FIXTagType type_;
    };

    using TagDefinitionsT = std::unordered_map<tag_id_t, FIXTagDefinition>;

}