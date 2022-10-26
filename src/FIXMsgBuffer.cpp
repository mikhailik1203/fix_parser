#include "FIXParserDefines.hpp"

#include <cstring>

using namespace fix;
using namespace std;

namespace{
    const char SOH = 1;    
    const int MSG_PREFIX_LEN = 12;
    const char MSG_PREFIX[] = {'8', '=', 'F', 'I', 'X', '.', '4', '.', '4', SOH, '9', '='};
    const int MAX_MSG_BODY_LEN = 9; //support messages with length less than 999999999 bytes 

    const int MSG_TYPE_LEN = 4;
    const char MSG_TYPE_PREFIX[] = {SOH, '3', '5', '='};
    const int MAX_MSG_TYPE_LEN = 16; 

    const int CRC_LEN = 4;
    const char CRC_PREFIX[] = {SOH, '1', '0', '='};
    const int MSG_CRC_LEN = 8; // SOH + 10=xxx + SOH
    const int MIN_MSG_LEN = MSG_PREFIX_LEN + 5 + MSG_CRC_LEN; //minimal msg body is '35=xSOH', 5 bytes
}

FIXMsgBuffer FIXMsgBuffer::create(const char *buffer, uint32_t length){
    FIXMsgBuffer data;
    bool len_valid = MIN_MSG_LEN < length;
    bool tag8_valid = len_valid && (0 == memcmp(buffer, MSG_PREFIX, MSG_PREFIX_LEN));
    if (!tag8_valid)
        return data;

    uint32_t msg_len = 0;
    uint32_t len_pos = MSG_PREFIX_LEN;
    while((len_pos < (MSG_PREFIX_LEN + MAX_MSG_BODY_LEN)) && isdigit(buffer[len_pos])){
        msg_len = msg_len*10 + (buffer[len_pos] - '0');
        ++len_pos;
    }
    // no sense to continue, if msg_len in correct or SOH was not found or 
    //  actual buffer length less than expected msg_len
    if(0 == msg_len || SOH != buffer[len_pos] || length < len_pos + 1 + msg_len + MSG_CRC_LEN)
        return data;

    // expecting next tag is MsgType(35)
    if (0 != memcmp(buffer + len_pos, MSG_TYPE_PREFIX, MSG_TYPE_LEN))
        return data;
    uint32_t type_pos_begin = len_pos + MSG_TYPE_LEN;        
    uint32_t type_pos = type_pos_begin;
    while(type_pos < (type_pos_begin + MAX_MSG_TYPE_LEN) && SOH != buffer[type_pos]){
        ++type_pos;
    }
    if(0 == type_pos - type_pos_begin)
        return data;

    data.type_ = FIXMsgType(buffer + type_pos_begin, type_pos - type_pos_begin);
    data.buffer_ = buffer + type_pos + 1; // skip MsgType tag and value 
    data.length_ = msg_len - (type_pos + MSG_TYPE_LEN - type_pos_begin);
    data.protocol_ = FIXProtocol::FIX44;
    return data;
}

FIXMsgBuffer FIXMsgBuffer::create(std::vector<char> &&data, size_t start_pos, 
                    const FIXMsgType &type, const FIXProtocol &protocol){
    FIXMsgBuffer msg_buf;
    msg_buf.data_ = std::move(data);    
    msg_buf.buffer_ = &msg_buf.data_[start_pos];
    msg_buf.length_ = msg_buf.data_.size() - start_pos;
    msg_buf.type_ = type;
    msg_buf.protocol_ = protocol;
    return msg_buf;
}