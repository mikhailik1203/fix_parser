#include "FIXParserDefines.hpp"

#include <cassert>

using namespace fix;

FIXMsgType::FIXMsgType(const char *val, uint32_t length){
    if(nullptr == val || 0 == length)
        return;
    
    switch(length){
    case 1:
        switch(val[0]){
        case 'A':
            type_ = FIXMessageType::Logon;
        break;
        case '0':
            type_ = FIXMessageType::HeartBeat;
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
            assert(false && "Message type is not supported(1 symbol)!");
        };
        break;
    case 2:
        if ('U' != val[0]){
            assert(false && "Message type is not supported(2 symbols)!");
        }else{
            type_ = FIXMessageType::UserDefined;                
        }
        break;
    default:
        if ('U' != val[0]){
            assert(false && "Message type is not supported - too long!");
        }else{
            type_ = FIXMessageType::UserDefined;                
        }
    }
    type_val_ = {std::string_view(val, length)};    
}