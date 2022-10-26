#pragma once

#include "FIXParserDefines.hpp"
#include "FIXMessageMetadata.hpp"
#include "FIXGroup.hpp"

namespace fix{
    class FIXDictionary;
    class FIXGroup;

    struct FIXMessage{
        friend FIXMessageMetadata;        
    public:
        explicit FIXMessage(const FIXMessageMetadata &meta);
        ~FIXMessage() = default;
        FIXMessage(const FIXMessage &) = delete;
        FIXMessage &operator=(const FIXMessage &) = delete;
        FIXMessage(FIXMessage &&);
        FIXMessage &operator=(FIXMessage &&) = delete;

        FIXProtocol protocol()const;
        const FIXMsgType &message_type()const;

    public:
        // serializes group entry data to the sequence of bytes
        void serialize(std::vector<char> &buffer)const;

    public:
        bool has_tag(tag_id_t tag)const;
        bool support_tag(tag_id_t tag)const;
        FIXTagType tag_type(tag_id_t tag)const;

    public:
        bool get_bool(tag_id_t tag)const;
        int get_int(tag_id_t tag)const;
        const FIXDouble &get_double(tag_id_t tag)const;
        const FIXString &get_string(tag_id_t tag)const;
        const FIXDate &get_date(tag_id_t tag)const;
        const FIXDatetime &get_datetime(tag_id_t tag)const;
        const FIXRawData &get_rawdata(tag_id_t tag)const;
        const FIXGroup &get_group(tag_id_t tag)const;
        FIXGroup &get_group(tag_id_t tag);

        bool set_bool(tag_id_t tag, bool val);
        bool set_int(tag_id_t tag, int val);
        bool set_double(tag_id_t tag, const FIXDouble &val);
        bool set_string(tag_id_t tag, const FIXString &val);
        bool set_date(tag_id_t tag, const FIXDate &val);
        bool set_datetime(tag_id_t tag, const FIXDatetime &val);
        bool set_rawdata(tag_id_t tag, const FIXRawData &val);

        void clear_tag(tag_id_t tag);

    protected:
        bool set_value(const TagMetadata &meta, tag_id_t tag, std::string_view val);

    private:
        const FIXMessageMetadata &meta_;
        FIXMsgType type_;

        // data
        std::string buffer_;
        std::vector<char> tag_bool_values_;
        std::vector<int> tag_int_values_;
        std::vector<FIXDouble> tag_double_values_;
        std::vector<FIXString> tag_string_values_;
        std::vector<FIXDate> tag_date_values_;
        std::vector<FIXDatetime> tag_datetime_values_;
        std::vector<FIXRawData> tag_rawdata_values_;
        std::vector<FIXGroup> tag_group_values_;
        BitMask available_tags_;
    };

}