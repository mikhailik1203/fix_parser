#pragma once

#include "FIXParserDefines.hpp"
#include "FIXMessage.hpp"

namespace fix{ 
    // Facade for FIX messages: creates FIX messages from scratch or raw text, serializes FIX message to the raw text
    class FIXParser{
    public:
        explicit FIXParser(const FIXDictionary &dict);
        ~FIXParser() = default;

        FIXParser(const FIXParser &) = delete;
        FIXParser &operator=(const FIXParser &) = delete;
        FIXParser(FIXParser &&) = delete;
        FIXParser &operator=(FIXParser &&) = delete;

        // creates and returns empty FIX message
        FIXMessage create(const std::string &type_name)const;

        // creates FIX message from raw text, 
        // Important: buffer is shared with FIXMessage object and must not be changed/destroyed while FIXMessage object is alive
        FIXMessage parse(const char *buffer, size_t len);
        
        // serializes FIX message to the raw text
        static FIXMsgBuffer serialize(const FIXMessage &msg, bool update_header = true, bool update_trailer = true);

    private:
        FIXMsgTypeMetadatasT msgtype_metadata_;
        FIXMsgMetadatasT usermsg_metadata_;
    };
}