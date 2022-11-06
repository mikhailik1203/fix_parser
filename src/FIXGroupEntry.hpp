#pragma once

#include "FIXParserDefines.hpp"
#include "FIXGroupMetadata.hpp"
#include "Utils.hpp"

#include <string_view>

namespace fix{
    class FIXDictionary;
    class FIXGroup;

    struct FIXGroupEntry{
        friend FIXGroupMetadata;
    public:
        explicit FIXGroupEntry(const FIXGroupMetadata &meta);
        ~FIXGroupEntry() = default;
        FIXGroupEntry(const FIXGroupEntry &) = delete;
        FIXGroupEntry &operator=(const FIXGroupEntry &) = delete;
        FIXGroupEntry(FIXGroupEntry &&)noexcept ;
        FIXGroupEntry &operator=(FIXGroupEntry &&) noexcept = delete;

        tag_id_t group_tag()const noexcept;
        entry_id_t entry_id()const noexcept;

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

        void clear_tag(tag_id_t tag);

    protected:
        bool set_value(const TagMetadata &meta, tag_id_t tag, MsgReceived &data);

    private:
        const FIXGroupMetadata &meta_;
        // data
        BitMask available_tags_;        
        std::vector<int> tag_int_values_;
        std::vector<FIXString> tag_string_values_;
        std::vector<FIXGroup> tag_group_values_;
        std::vector<char> tag_char_values_;
        std::vector<FIXDouble> tag_double_values_;
        std::vector<FIXDate> tag_date_values_;
        std::vector<FIXDatetime> tag_datetime_values_;
        std::vector<FIXRawData> tag_rawdata_values_;
    };

    static FIXGroupEntry DUMMY_GROUP_ENTRY(DUMMY_GROUP_METADATA);
}