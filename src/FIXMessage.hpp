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
        FIXMessage(FIXMessage &&)noexcept ;
        FIXMessage &operator=(FIXMessage &&) noexcept = delete;

        FIXProtocol protocol()const noexcept;
        const FIXMsgType &message_type()const noexcept;

    public:
        // serializes group entry data to the sequence of bytes
        void serialize(MsgSerialised &buffer)const;

    public:
        bool has_tag(tag_id_t tag)const noexcept;
        bool support_tag(tag_id_t tag)const noexcept;
        FIXTagType tag_type(tag_id_t tag)const noexcept;

    public:
        bool get_bool(tag_id_t tag)const noexcept;
        char get_char(tag_id_t tag)const noexcept;
        int get_int(tag_id_t tag)const noexcept;
        const FIXDouble &get_double(tag_id_t tag)const noexcept;
        const FIXString &get_string(tag_id_t tag)const noexcept;
        const FIXDate &get_date(tag_id_t tag)const noexcept;
        const FIXDatetime &get_datetime(tag_id_t tag)const noexcept;
        const FIXRawData &get_rawdata(tag_id_t tag)const noexcept;
        const FIXGroup &get_group(tag_id_t tag)const noexcept;
        FIXGroup &get_group(tag_id_t tag);

        bool set_bool(tag_id_t tag, bool val);
        bool set_char(tag_id_t tag, char val);
        bool set_int(tag_id_t tag, int val);
        bool set_double(tag_id_t tag, const FIXDouble &val);
        bool set_string(tag_id_t tag, const FIXString &val);
        bool set_date(tag_id_t tag, const FIXDate &val);
        bool set_datetime(tag_id_t tag, const FIXDatetime &val);
        bool set_rawdata(tag_id_t tag, const FIXRawData &val);

        void clear_tag(tag_id_t tag) noexcept;

    protected:
        bool set_value(const TagMetadata &meta, tag_id_t tag, MsgReceived &data);

    private:
        const FIXMessageMetadata &meta_;

        BitMask available_tags_;
        std::vector<int> tag_int_values_;
        std::vector<FIXString> tag_string_values_;
        std::vector<FIXGroup> tag_group_values_;
        std::vector<char> tag_char_values_;
        std::vector<FIXDouble> tag_double_values_;
        std::vector<FIXDate> tag_date_values_;
        std::vector<FIXDatetime> tag_datetime_values_;
        std::vector<FIXRawData> tag_rawdata_values_;
        std::vector<char> raw_msg_data_;
    };

    const FIXMessage DUMMY_FIX_MESSAGE(DUMMY_FIXMSG_METADATA);
}