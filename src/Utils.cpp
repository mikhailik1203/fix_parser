#include "Utils.hpp"

#include <cassert>

namespace fix{
    void to_string(int val, std::vector<char> &buffer){
        if(0 == val){
            char buf[] = "0\001";
            buffer.insert(buffer.end(), buf, buf + 2);
            return;
        }else{
            char buf[16];
            int pos = 0;
            if(val < 0){
                buf[pos++] = '-';
                val *= -1;
            }
            if(val < 100000){
                if(val < 100){
                    pos += (val >= 10)?(2):(1);
                }else{
                    if(val < 10000){
                        pos += (val >= 1000)?(4):(3);
                    }else{
                        pos += 5;
                    }
                }
            }else{
                if(val < 10000000){
                    pos += (val >= 1000000)?(7):(6);
                }else{
                    if(val < 1000000000){
                        pos += (val >= 100000000)?(9):(8);
                    }else{
                        pos += 10;    
                    }
                }
            }

            buf[pos--] = 1;
            auto count = pos + 1;
            while (0 < val){
                buf[pos--] = '0' + val%10;
                val /= 10;
            }
            buffer.insert(buffer.end(), buf, buf + count + 1);
        }
    }

    std::string to_tag_prefix(int val){
        return std::to_string(val) + "=";
    }

    int string_to_int_positive(const std::string_view &val){
        int res = 0;
        for(auto v: val){
            res = res*10 + (v - '0');
        }
        return res;
    }

    int string_to_int(const std::string_view &val){
        int res = 0;
        size_t count = val.size();
        size_t pos = 0;
        int mul = 1;
        if('-' == val[pos]){
            mul = -1;
            ++pos;
        }
        while(pos != count){
            unsigned digit = val[pos] - '0';
            if(digit <= 9){
                res = res*10 + digit;
            }else{
                return mul*res;
            }
            ++pos;
        }
        return mul*res;
    }

    static const uint64_t BITS[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 1ULL<< 12, 1ULL<< 13, 1ULL<< 14, 1ULL<< 15, 1ULL<< 16, 1ULL<<17, 
            1ULL<< 18, 1ULL<< 19, 1ULL<< 20, 1ULL<< 21, 1ULL<< 22, 1ULL<< 23, 1ULL<< 24, 1ULL<< 25, 1ULL<< 26, 1ULL<< 27, 1ULL<< 28, 1ULL<< 29, 1ULL<< 30, 
            1ULL<< 31, 1ULL<< 32, 1ULL<< 33, 1ULL<< 34, 1ULL<< 35, 1ULL<< 36, 1ULL<< 37, 1ULL<< 38, 1ULL<< 39, 1ULL<< 40, 1ULL<< 41, 1ULL<< 42, 1ULL<< 43, 
            1ULL<< 44, 1ULL<< 45, 1ULL<< 46, 1ULL<< 47, 1ULL<< 48, 1ULL<< 49, 1ULL<< 50, 1ULL<< 51, 1ULL<< 52, 1ULL<< 53, 1ULL<< 54, 1ULL<< 55, 1ULL<< 56,
            1ULL<< 57, 1ULL<< 58, 1ULL<< 59, 1ULL<< 60, 1ULL<< 61, 1ULL<< 62, 1ULL<< 63};
}

using namespace fix;

size_t BitMask::size()const noexcept{
    return 8*sizeof(mask_);
}

void BitMask::set(size_t index){
    assert(index < 8*sizeof(mask_));
    mask_ |= BITS[index];
}

void BitMask::unset(size_t index){
    assert(index < 8*sizeof(mask_));
    mask_ &= ~BITS[index];
}

bool BitMask::is_set(size_t index)const noexcept{
    return 0 != (BITS[index] & mask_);
}

size_t MsgReceived::current_tag_id(){
    if(0 != curr_tag_id_)
        return curr_tag_id_;
    curr_tag_id_ = 0;
    while(pos_ < data_.size()){
        int val = data_[pos_] - '0';
        if (0 <= val && val <= 9){
            curr_tag_id_ = 10*curr_tag_id_ + val;
        }else if(13 == val){ //'=' symbol
            ++pos_;// skip '=', so next is a value symbol
            return curr_tag_id_;
        }else{
            break;
        }
        ++pos_;
    }
    curr_tag_id_ = 0;
    error_ = "Invalid tag-value format, '=' is expected after tag_id!";
    return curr_tag_id_;
}

std::string_view MsgReceived::parse_value(){
    curr_tag_id_ = 0;
    size_t start_pos_ = pos_;
    while(pos_ < data_.size()){
        if(1 == data_[pos_]){
            ++pos_;// skip SOH
            return std::string_view(&data_[start_pos_], pos_ - start_pos_ - 1);
        }
        ++pos_;
    }
    error_ = "Invalid tag-value format, SOH is expected after value!";
    return std::string_view();
}

std::string_view MsgReceived::parse_rawdata_value(int raw_len){
    curr_tag_id_ = 0;
    size_t start_pos_ = pos_;
    if(start_pos_ + raw_len < data_.size()){
        pos_ = pos_ + raw_len + 1;// skip raw_len and SOH
        return std::string_view(&data_[start_pos_], pos_ - start_pos_ - 1);
    }
    error_ = "Invalid RawData format, data should contains raw_len and SOH!";
    return std::string_view();
}

bool MsgReceived::parse_bool_value(){
    curr_tag_id_ = 0;
    size_t curr_pos_ = pos_++;
    if(1 == data_[pos_]){
        ++pos_;// skip SOH
        return 'Y' == data_[curr_pos_];
    }
    error_ = "Invalid bool tag-value format, SOH is expected after value!";
    return false; 
}

char MsgReceived::parse_char_value(){
    curr_tag_id_ = 0;
    size_t curr_pos_ = pos_++;
    if(1 == data_[pos_]){
        ++pos_;// skip SOH
        return data_[curr_pos_];
    }
    error_ = "Invalid char tag-value format, SOH is expected after value!";
    return 0;
}

int MsgReceived::parse_int_value(){
    curr_tag_id_ = 0;

    int res = 0;
    size_t count = data_.size();
    int mul = 1;
    if('-' == data_[pos_]){
        mul = -1;
        ++pos_;
    }
    while(pos_ != count){
        unsigned digit = data_[pos_] - '0';
        if(digit <= 9){
            res = res*10 + digit;
        }else{
            if(1 == data_[pos_]){
                ++pos_;// skip SOH
                return mul*res;
            }
            error_ = "Invalid int tag-value format, SOH is expected after value!";
            return 0;
        }
        ++pos_;
    }
    error_ = "Invalid int tag-value format, SOH is not found in message!";    
    return 0;
}

