#pragma once

#include "FIXParserDefines.hpp"
#include "FIXMessage.hpp"

namespace fix{
    class FIXParser{
    public:
        FIXParser(const FIXDictionary &dict);
        ~FIXParser() = default;

        FIXParser(const FIXParser &) = delete;
        FIXParser &operator=(const FIXParser &) = delete;
        FIXParser(FIXParser &&) = delete;
        FIXParser &operator=(FIXParser &&) = delete;

        FIXMessage parse(const FIXMsgBuffer &msg_buffer);
        static FIXMsgBuffer serialize(const FIXMessage &msg, bool update_header = true, bool update_trailer = true);

    private:
        FIXMsgMetadatasT msg_metadata_;
    };
}