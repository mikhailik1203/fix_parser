#include "FIXParser.hpp"
#include "./dictionary/FIXDictionary.hpp"

#include <cassert>

using namespace fix;

FIXParser::FIXParser(const FIXDictionary &dict){
    for(const auto &[msg_name, msg_def]: dict.messages()){
        FIXMsgType msg_type(msg_name.c_str(), msg_name.size());
        auto type = msg_type.type();
        if(FIXMessageType::UserDefined != type && FIXMessageType::CustomDefined != type){
            msgtype_metadata_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(type),
                    std::forward_as_tuple(msg_name, dict));
        }else{
            usermsg_metadata_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(msg_name),
                    std::forward_as_tuple(msg_name, dict));
        }
    }
}

FIXMessage FIXParser::create(const std::string &type_name)const{
    FIXMsgType msg_type(type_name.c_str(), type_name.size());   
    auto type = msg_type.type();
    if(FIXMessageType::UserDefined != type && FIXMessageType::CustomDefined != type){     
        auto meta_it = msgtype_metadata_.find(msg_type.type());
        if(msgtype_metadata_.end() != meta_it){
            return meta_it->second.create();    
        }
    }else{
        auto meta_it = usermsg_metadata_.find(type_name);
        if(usermsg_metadata_.end() != meta_it){
            return meta_it->second.create();    
        }
    }
    return FIXMessage(DUMMY_FIXMSG_METADATA);    
} 

FIXMessage FIXParser::parse(const char *buffer, size_t len){
    FIXMsgBuffer msg_buffer = FIXMsgBuffer::create(buffer, len);
    auto type = msg_buffer.type().type();
    if(FIXMessageType::UserDefined != type && FIXMessageType::CustomDefined != type){     
        auto meta_it = msgtype_metadata_.find(type);
        if(msgtype_metadata_.end() != meta_it){
            MsgReceived msg_data{msg_buffer.buffer(), 0, "", 0};
            return meta_it->second.parse(msg_data);
        }
    }else{
        auto meta_it = usermsg_metadata_.find(std::string(msg_buffer.type().type_value().to_string()));
        if(usermsg_metadata_.end() != meta_it){
            MsgReceived msg_data{msg_buffer.buffer(), 0, "", 0};
            return meta_it->second.parse(msg_data);
        }
    }
    return FIXMessage(DUMMY_FIXMSG_METADATA);
}

FIXMsgBuffer FIXParser::serialize(const FIXMessage &msg, bool update_header, bool update_trailer){
    const size_t PREFIX_SIZE = 36;
    const size_t BUFFER_SIZE = 256;
    MsgSerialised msg_buffer(BUFFER_SIZE, PREFIX_SIZE);
    msg.serialize(msg_buffer);
    if(!msg_buffer.error_.empty()){
        throw std::runtime_error(msg_buffer.error_);
    }
    auto &buffer = msg_buffer.buffer_;
    // add message prefix 8 and 9 tag, add suffix - tag 10
    //  '8=FIX.4.4<SOH>9=<number bytes><SOH>....10=xxx<SOH>'
    size_t msg_body_length = msg_buffer.pos_ - PREFIX_SIZE;
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

    char suffix[] = "10=000";
    const size_t suffix_len = 6;
    { /// calculate CRC summ and update suffix buffer
        unsigned crc = 0;
        auto end_pos = msg_buffer.pos_;
        for(size_t pos = msg_start; pos != end_pos; ++pos){
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
    msg_buffer.append(std::string_view(suffix, suffix_len));
    return FIXMsgBuffer::create(std::move(buffer), msg_start, msg_buffer.pos_, msg.message_type(), msg.protocol());
}
