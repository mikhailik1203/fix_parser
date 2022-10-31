#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace fix{

    class BitMask{
    public:
        size_t size()const noexcept;
        void set(size_t index);
        void unset(size_t index);
        bool is_set(size_t index)const noexcept;
    private:
        uint64_t mask_ = 0;
    };

    // Helper class for parsing: contains buffer with application tags and values (tags 8, 9, 35 are excluded)
    struct MsgReceived{
        const std::string_view data_; //received buffer
        size_t pos_ = 0; //current parsing position
        std::string error_; // error message in case of invalid message
        size_t curr_tag_id_ = 0;

        // parses tag_id from current position and skips following '=' symbol, so next is value's symbol
        // returns 0 and sets error_ field in case of error
        size_t current_tag_id();
        
        // parses value till SOH symbol and returns start position and size
        // returns empty string_view and sets error_ field in case of error
        std::string_view parse_value();

        // parses value till len and returns start position and size
        // returns empty string_view and sets error_ field in case of error
        std::string_view parse_rawdata_value(int raw_len);

        bool parse_bool_value();
        char parse_char_value();
        int parse_int_value();
    };

    // serialize val to the string and adds <SOH> to the end
    void to_string(int val, std::vector<char> &buffer);

    std::string to_tag_prefix(int val);

    // parses val to the integer value
    int string_to_int_positive(const std::string_view &val);
    int string_to_int(const std::string_view &val);
}