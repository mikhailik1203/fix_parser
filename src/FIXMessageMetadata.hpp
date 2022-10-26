#pragma once

#include "FIXParserDefines.hpp"
#include "./dictionary/FIXBlockDefinition.hpp"
#include "./dictionary/FIXTagVocabulary.hpp"
#include "FIXGroupMetadata.hpp"
#include "Utils.hpp"

#include <unordered_map>

namespace fix{
    class FIXDictionary;
    class FIXMessage;

    // Contains FIX message template: structure, containers with values for tags and groups
    class FIXMessageMetadata{
    public:
        FIXMessageMetadata(const std::string &name, const FIXDictionary &dict);

        FIXProtocol protocol()const noexcept{return protocol_;}
        const std::string &name()const noexcept{return name_;}
        location_id_t location_id()const noexcept{return location_id_;}

        // creates empty instance of group
        FIXMessage create()const;

        // creates instance of group from FIX message
        FIXMessage parse(MsgReceived &data)const;

    public:
        bool support_tag(tag_id_t tag)const;
        FIXTagType tag_type(tag_id_t tag)const;

        size_t tag_bool_count()const noexcept{return tag_bool_count_;}
        size_t tag_int_count()const noexcept{return tag_int_count_;}
        size_t tag_double_count()const noexcept{return tag_double_count_;}
        size_t tag_string_count()const noexcept{return tag_string_count_;}
        size_t tag_date_count()const noexcept{return tag_date_count_;}
        size_t tag_datetime_count()const noexcept{return tag_datatime_count_;}
        size_t tag_rawdata_count()const noexcept{return tag_rawdata_count_;}
        const std::vector<FIXGroupMetadata> &groups()const noexcept{return tag_group_values_;}

        const TagMetadata &tag_metadata(tag_id_t tag)const noexcept;

        size_t tags_count()const noexcept{ return tag_metadata_.size();}
    public:
        void serialize(const FIXMessage &data, std::vector<char> &buffer)const;

    private:
        void process_tag(const FIXDictionary &dict, const FIXTagVocabulary &vocab, tag_id_t tag_id);
        void process_block(const FIXDictionary &dict, const FIXTagVocabulary &vocab, block_id_t block_id);
        bool set_tag_value(const fix::TagMetadata &meta_data, size_t &tag_id, std::string_view val, 
                                     MsgReceived &data, FIXMessage &msg)const;

    private:
        std::string name_;
        FIXProtocol protocol_;
        location_id_t location_id_;

        using TagsMetadataT = std::vector<TagMetadata>; // order of tags is important
        using TagToMetaIndexT = std::unordered_map<tag_id_t, int>; //tag_id to the index
        TagsMetadataT tag_metadata_; 
        TagToMetaIndexT tag_to_meta_idx_;

        std::string buffer_;
        uint16_t tag_bool_count_ = 0;
        uint16_t tag_int_count_ = 0;
        uint16_t tag_double_count_ = 0;
        uint16_t tag_string_count_ = 0;
        uint16_t tag_date_count_ = 0;
        uint16_t tag_datatime_count_ = 0;
        uint16_t tag_rawdata_count_ = 0;
        std::vector<FIXGroupMetadata> tag_group_values_;        
    };

    using FIXMsgMetadatasT = std::unordered_map<std::string, FIXMessageMetadata>;
}