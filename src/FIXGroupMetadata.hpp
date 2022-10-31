#pragma once

#include "FIXParserDefines.hpp"
#include "./dictionary/FIXBlockDefinition.hpp"
#include "Utils.hpp"

#include <unordered_map>

namespace fix{
    class FIXDictionary;
    class FIXTagVocabulary;
    class FIXGroup;
    class FIXGroupEntry;

    // Contains FIX group template: structure, containers with values for tags and groups
    class FIXGroupMetadata{
    public:
        FIXGroupMetadata() = default;
        FIXGroupMetadata(tag_id_t leading_tag_id, const FIXDictionary &dict);
        ~FIXGroupMetadata() = default;
        FIXGroupMetadata(const FIXGroupMetadata &) = default;
        FIXGroupMetadata &operator=(const FIXGroupMetadata &) = default;
        FIXGroupMetadata(FIXGroupMetadata &&) = default;
        FIXGroupMetadata &operator=(FIXGroupMetadata &&) = default;

        // returns leading tag of the group
        inline tag_id_t leading_tag()const noexcept{return leading_tag_id_;}
        inline tag_id_t entry_tag()const noexcept{return entry_start_tag_id_;}
        inline location_id_t location_id()const noexcept{return location_id_;}

        // creates empty instance of group
        FIXGroup create()const;

        // creates instance of group from FIX message
        FIXGroup parse(MsgReceived &data)const;
        bool parse(MsgReceived &data, FIXGroup &grp_val, size_t entry_count)const;

        // serializes data to the sequence of bytes
        void serialize_group_size(size_t count,  std::vector<char> &buffer)const;

        std::vector<FIXGroup> create_nested()const;
    public:
        bool support_tag(tag_id_t tag)const noexcept;
        FIXTagType tag_type(tag_id_t tag)const noexcept;

        inline size_t tag_char_count()const noexcept{return tag_char_count_;}
        inline size_t tag_int_count()const noexcept{return tag_int_count_;}
        inline size_t tag_double_count()const noexcept{return tag_double_count_;}
        inline size_t tag_string_count()const noexcept{return tag_string_count_;}
        inline size_t tag_date_count()const noexcept{return tag_date_count_;}
        inline size_t tag_datetime_count()const noexcept{return tag_datatime_count_;}
        inline size_t tag_rawdata_count()const noexcept{return tag_rawdata_count_;}
        const std::vector<std::pair<FIXGroupMetadata, size_t>> &groups()const noexcept{return tag_group_values_;}

        const TagMetadata &tag_metadata(tag_id_t tag)const noexcept;

        size_t tags_count()const noexcept{ return tag_metadata_.size();}
    public:
        void serialize(const FIXGroupEntry &data, std::vector<char> &buffer)const;

    private:
        void process_tag(const FIXDictionary &dict, const FIXTagVocabulary &vocab, tag_id_t tag_id);
        void process_block(const FIXDictionary &dict, const FIXTagVocabulary &vocab, block_id_t block_id);
        bool set_tag_value(const fix::TagMetadata &meta, size_t &tag_id, MsgReceived &data, FIXGroupEntry &entry, FIXGroup &grp)const;
        bool process_entry(size_t &tag_id, MsgReceived &data, FIXGroupEntry &entry, FIXGroup &grp)const;

    private:
        tag_id_t leading_tag_id_ = 0;
        tag_id_t entry_start_tag_id_ = 0;
        location_id_t location_id_ = 0;
        std::string leading_tag_prefix_;

        using TagsMetadataT = std::vector<TagMetadata>; // order of tags is important
        using TagToMetaIndexT = std::unordered_map<tag_id_t, int>; //tag_id to the index
        TagsMetadataT tag_metadata_; 
        TagToMetaIndexT tag_to_meta_idx_;

        // templates for data
        //std::string buffer_;
        uint16_t tag_char_count_ = 0;
        uint16_t tag_int_count_ = 0;
        uint16_t tag_double_count_ = 0;
        uint16_t tag_string_count_ = 0;
        uint16_t tag_date_count_ = 0;
        uint16_t tag_datatime_count_ = 0;
        uint16_t tag_rawdata_count_ = 0;
        std::vector<std::pair<FIXGroupMetadata, size_t>> tag_group_values_;
    };

    using FIXGrpTmplatesT = std::unordered_map<tag_id_t, FIXGroupMetadata>;

    static FIXGroupMetadata DUMMY_GROUP_METADATA;
    
}
