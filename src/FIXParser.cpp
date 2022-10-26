#include "FIXParser.hpp"
#include "./dictionary/FIXDictionary.hpp"

#include <cassert>

using namespace fix;

FIXParser::FIXParser(const FIXDictionary &dict){
    for(const auto &[msg_name, msg_def]: dict.messages()){
        msg_metadata_.emplace(msg_name, FIXMessageMetadata{msg_name, dict});
    }
}

FIXMessage FIXParser::create(const std::string &type_name)const{
    auto meta_it = msg_metadata_.find(type_name);
    if(msg_metadata_.end() == meta_it){
        return FIXMessage(DUMMY_FIXMSG_METADATA);
    }
    return meta_it->second.create();
}

FIXMessage FIXParser::parse(const FIXMsgBuffer &msg_buffer){
    const std::string_view &type_val = msg_buffer.type().type_value().to_string();
    auto meta_it = msg_metadata_.find(std::string(type_val));
    if(msg_metadata_.end() == meta_it){
        return FIXMessage(DUMMY_FIXMSG_METADATA);
    }
    MsgReceived msg_data{msg_buffer.buffer(), 0, "", 0};
    return meta_it->second.parse(msg_data);
}

FIXMsgBuffer FIXParser::serialize(const FIXMessage &msg, bool update_header, bool update_trailer){
    const size_t PREFIX_SIZE = 36;
    const size_t BUFFER_SIZE = 256;
    std::vector<char> buffer(PREFIX_SIZE, 0);
    buffer.reserve(BUFFER_SIZE);
    msg.serialize(buffer);
    // add message prefix 8 and 9 tag, add suffix - tag 10
    //  '8=FIX.4.4<SOH>9=<number bytes><SOH>....10=xxx<SOH>'
    size_t msg_body_length = buffer.size() - PREFIX_SIZE;
    size_t msg_length = msg_body_length;
    size_t pos = PREFIX_SIZE - 1;
    buffer[pos--] = 1; // add SOH after length value
    while (0 != msg_length){
        char val = msg_length%10;
        buffer[pos--] = '0' + val;
        msg_length /= 10;
    }
    size_t msg_start = 0;
    switch(msg.protocol()){
        case FIXProtocol::FIX44:{
            const char prefix[] = "8=FIX.4.4\0019=";
            const size_t prefix_len = 12;
            std::copy(prefix, prefix + prefix_len, buffer.begin() + pos - prefix_len + 1); 
            msg_start = pos - prefix_len + 1;
        }
        break;
        case FIXProtocol::FIXTest:{
            const char prefix[] = "8=FIX.TEST\0019=";
            const size_t prefix_len = 13;
            std::copy(prefix, prefix + prefix_len, buffer.begin() + pos - prefix_len + 1); 
            msg_start = pos - prefix_len + 1;
        }
        break;
        default:
            assert(false && "Unable to serialize message - uknown protocol!");
            return FIXMsgBuffer::create_dummy();
    }

    char suffix[] = "10=000\001";
    const size_t suffix_len = 7;
    { /// calculate CRC summ and update suffix buffer
        unsigned crc = 0;
        for(size_t pos = msg_start; pos != buffer.size(); ++pos){
            crc += buffer[pos];
        }
        crc = crc%256;
        suffix[5] += crc % 10;
        crc = crc / 10;
        suffix[4] += crc % 10;
        crc = crc / 10;
        suffix[3] += crc % 10;
        crc = crc / 10;
    }
    buffer.insert(buffer.end(), suffix, suffix + suffix_len); 
    return FIXMsgBuffer::create(std::move(buffer), msg_start, msg.message_type(), msg.protocol());
}
