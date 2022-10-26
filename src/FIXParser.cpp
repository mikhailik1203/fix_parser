#include "FIXParser.hpp"
#include "./dictionary/FIXDictionary.hpp"

using namespace fix;

FIXParser::FIXParser(const FIXDictionary &dict){
    for(const auto &[msg_name, msg_def]: dict.messages()){
        msg_metadata_.emplace(msg_name, FIXMessageMetadata{msg_name, dict});
    }
}

FIXMessage FIXParser::parse(const FIXMsgBuffer &msg_buffer){
    const std::string_view &type_val = msg_buffer.type().type_value().to_string();
    auto meta_it = msg_metadata_.find(std::string(type_val));
    if(msg_metadata_.end() == meta_it){
        throw std::runtime_error("Unable to parse message - uknown message!");
    }
    MsgReceived msg_data{msg_buffer.buffer(), 0, "", 0};
    return meta_it->second.parse(msg_data);
}

FIXMsgBuffer FIXParser::serialize(const FIXMessage &msg, bool update_header, bool update_trailer){
    const size_t PREFIX_SIZE = 36;
    std::vector<char> buffer(PREFIX_SIZE, 0);
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
            throw std::runtime_error("Unable to serialize message - uknown protocol!");
    }
    const char suffix[] = "10=000\001";
    // calculate CheckSumm value and update suffix
    const size_t suffix_len = 7;
    buffer.insert(buffer.end(), suffix, suffix + suffix_len); 
    return FIXMsgBuffer::create(std::move(buffer), msg_start, msg.message_type(), msg.protocol());
}
