#include "FIXParserDefines.hpp"

#include <cassert>

using namespace fix;

FIXMsgType::FIXMsgType(const char *val, uint32_t length){
    if(nullptr == val || 0 == length)
        return;
    type_ = FIXMessageType::CustomDefined;        
    switch(length){
    case 1:
        switch(val[0]){
        case 'A':
            type_ = FIXMessageType::Logon;
        break;
        case '0':
            type_ = FIXMessageType::HeartBeat;
        break;
        case '1':
            type_ = FIXMessageType::TestRequest;
        break;
        case '2':
            type_ = FIXMessageType::ResendRequest;
        break;
        case '3':
            type_ = FIXMessageType::Reject;
        break;
        case '4':
            type_ = FIXMessageType::SequenceReset;
        break;
        case '5':
            type_ = FIXMessageType::Logout;
        break;
        case 'D':
            type_ = FIXMessageType::NewOrderSingle;
        break;
        case 'V':
            type_ = FIXMessageType::MarketDataRequest;
        break;
        case 'U':
            type_ = FIXMessageType::UserDefined;
        break;
        default:
            ; // it's CustomDefined message
        };
        break;
    case 2:
        if ('U' == val[0]){
            type_ = FIXMessageType::UserDefined;
        } 
        break;
    default:
        if ('U' == val[0]){
            type_ = FIXMessageType::UserDefined;
        }
    }
    type_val_ = {std::string_view(val, length)};    
}