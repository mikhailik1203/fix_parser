#include "Utils.hpp"

#include <cassert>

namespace fix{
    void to_string(int val, std::vector<char> &buffer){
        std::string str = std::to_string(val);
        buffer.insert(buffer.end(), str.begin(), str.end());
        buffer.push_back(1);
    }

    int string_to_int(const std::string_view &val){
        int res = 0;
        for(auto v: val){
            res = res*10 + (v - '0');
        }
        return res;
    }
}

using namespace fix;

size_t BitMask::size()const noexcept{
    return 8*sizeof(mask_);
}

void BitMask::set(size_t index){
    assert(index < 8*sizeof(mask_));
    int64_t val = 1 << index;
    mask_ |= val;
}

void BitMask::unset(size_t index){
    assert(index < 8*sizeof(mask_));
    int64_t val = 1 << index;
    mask_ &= ~val;
}

bool BitMask::is_set(size_t index)const noexcept{
    int64_t val = 1 << index;
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
    auto raw_len = string_to_int(len_val);
    curr_tag_id_ = 0;
    size_t start_pos_ = pos_;
    if(start_pos_ + raw_len < data_.size()){
        pos_ = pos_ + raw_len + 1;// skip raw_len and SOH
        return std::string_view(&data_[start_pos_], pos_ - start_pos_ - 1);
    }
    error_ = "Invalid RawData format, data should contains raw_len and SOH!";
    return std::string_view();
}

