#pragma once

#include <string>
#include <set>
#include <vector>

namespace fix{
    enum class FIXProtocol{
        invalid = 0,
        FIX44,
        FIXTest, //it is used for unit testing
    };

    enum class FIXMessageType{
        invalid = 0,
        Logon,
        HeartBeat,
        Logout,

        NewOrderSingle,

        MarketDataRequest,

        UserDefined
    };

    using tag_id_t = uint32_t;
    using location_id_t = int32_t;
    using block_id_t = int32_t;
    using entry_id_t = uint32_t;
    using entry_size_t = uint32_t;

    using SupportedTagsT = std::vector<tag_id_t>;
    using SupportedBlocksT = std::vector<block_id_t>;
    using SupportedGroupsT = std::vector<tag_id_t>;
    using RelatedMessagesT = std::set<std::string>;
    using RelatedGroupsT = std::set<tag_id_t>;
    using RelatedBlocksT = std::set<block_id_t>;

    enum class FIXTagType{
        invalid = 0,
        BOOL,
        CHAR,
        INT,
        DOUBLE,
        STRING,
        DATE,
        DATETIME,
        RAWDATALEN,
        RAWDATA,
        GROUP
    };

    struct FIXString{
        const std::string_view &value()const{return value_;}
        const std::string_view &to_string()const{return value_;}
    public:
        std::string_view value_;
    };

    struct FIXMsgType{
    public:
        FIXMsgType() = default;
        FIXMsgType(const char *val, uint32_t length);

        FIXMessageType type()const noexcept{return type_;}
        const FIXString &type_value()const noexcept{ return type_val_;}
    private:
        FIXString type_val_;
        FIXMessageType type_ = FIXMessageType::invalid;
    };


    // contains data buffer with FIX message and preparsed FIX parameters, used to construct FIXMessage
    struct FIXMsgBuffer{
    public:
        static FIXMsgBuffer create_dummy();
        static FIXMsgBuffer create(const char *buffer, uint32_t length);
        static FIXMsgBuffer create(std::vector<char> &&data, size_t start_pos, const FIXMsgType &type, const FIXProtocol &protocol);

        /// length of the message (number of bytes)
        uint32_t length()const noexcept{ return length_;}

        // FIX protocol of the message
        FIXProtocol protocol()const noexcept{ return protocol_;}

        // FIX message type
        const FIXMsgType &type()const noexcept{ return type_;}

        // returns buffer with FIX message from first tag till last
        const char *buffer()const noexcept{ return buffer_;}
    private:
        std::vector<char> data_;
        FIXMsgType type_;
        const char *buffer_ = nullptr;
        uint32_t length_ = 0;
        FIXProtocol protocol_ = FIXProtocol::invalid;
    };

    struct FIXDouble{
        double value()const;
        const std::string_view &to_string()const{return value_;}
    public:
        std::string_view value_;
    };

    struct FIXDate{
        uint32_t year()const;
        u_int8_t month()const;
        u_int8_t day()const;

        const std::string_view &to_string()const{return value_;}
    public:
        std::string_view value_;
    };

    struct FIXDatetime{
        uint32_t year()const;
        u_int8_t month()const;
        u_int8_t day()const;
        u_int8_t hour()const;
        u_int8_t minute()const;
        u_int8_t seconds()const;
        u_int16_t mseconds()const;
        const std::string_view &to_string()const{return value_;}
    public:
        std::string_view value_;
    };

    struct FIXRawData{
        uint32_t size()const;
        char at(uint32_t index)const;
        const std::string_view &to_string()const{return value_;}
    public:
        std::string_view value_;
    };

    struct TagMetadata{
        tag_id_t tag_id_ = 0;
        FIXTagType type_ = FIXTagType::invalid;
        size_t value_index_ = 0; // index o the value in tag_*_values_ container
        size_t tag_position_ = 0;
        std::string tag_prefix_;
    };

}