#include "Utils.hpp"

#include <cassert>

namespace fix{
    void to_string(int val, std::vector<char> &buffer){
        if(0 == val){
            buffer.push_back('0');
            buffer.push_back(1);
            return;
        }
        char buf[16];
        int pos = 0;
        if(val < 0){
            buf[pos++] = '-';
            val *= -1;
        }
        if(val >= 1000000000){
            pos += 10;
        }else if(val >= 100000000){
            pos += 9;
        }else if(val >= 10000000){
            pos += 8;
        }else if(val >= 1000000){
            pos += 7;
        }else if(val >= 100000){
            pos += 6;
        }else if(val >= 10000){
            pos += 5;
        }else if(val >= 1000){
            pos += 4;
        }else if(val >= 100){
            pos += 3;
        }else if(val >= 10){
            pos += 2;
        }else{
            pos += 1;
        }
        buf[pos--] = 1;
        auto count = pos + 1;
        while (0 < val){
            buf[pos--] = '0' + val%10;
            val /= 10;
        }
        buffer.insert(buffer.end(), buf, buf + count + 1);
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
    //int64_t val = 1 << index;
    int64_t val = BITS[index];
    mask_ |= val;
}

void BitMask::unset(size_t index){
    assert(index < 8*sizeof(mask_));
    //int64_t val = 1 << index;
    int64_t val = BITS[index];
    mask_ &= ~val;
}

bool BitMask::is_set(size_t index)const noexcept{
    //int64_t val = 1 << index;
    int64_t val = BITS[index];
    val = val & mask_;
    return 0 != val;
}

size_t MsgReceived::current_tag_id(){
    if(0 != curr_tag_id_)
        return curr_tag_id_;
    curr_tag_id_ = 0;
    while(pos_ < data_.size()){
        if ('0' <= data_[pos_] && data_[pos_] <= '9'){
            curr_tag_id_ = 10*curr_tag_id_ + (data_[pos_] - '0');
        }else if('=' == data_[pos_]){
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

std::string_view MsgReceived::parse_rawdata_value(const std::string_view &len_val){
    auto raw_len = string_to_int_positive(len_val);
    curr_tag_id_ = 0;
    size_t start_pos_ = pos_;
    if(start_pos_ + raw_len < data_.size()){
        pos_ = pos_ + raw_len + 1;// skip raw_len and SOH
        return std::string_view(&data_[start_pos_], pos_ - start_pos_ - 1);
    }
    error_ = "Invalid RawData format, data should contains raw_len and SOH!";
    return std::string_view();
}

