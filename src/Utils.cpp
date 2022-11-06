#include "Utils.hpp"

#include <cassert>
#include <charconv>

namespace fix{
    void to_string(int val, std::vector<char> &buffer){
        if(0 == val){
            char buf[] = "0\001";
            buffer.insert(buffer.end(), buf, buf + 2);
            return;
        }else{
            auto last_pos = buffer.size();
            if(buffer.capacity() - last_pos < 16){
                buffer.resize(buffer.size() + 16);
            }

            auto [last_ptr, err] = std::to_chars(buffer.data() + last_pos, buffer.data() + last_pos + 15, val);
            if(std::errc() == err){
                last_ptr[0] = 1;
                buffer.resize(last_ptr - buffer.data() + 1);
                return;
            }
            assert(false && "to_string: Unable to convert value to string!");
        }
    }

    std::string to_tag_prefix(int val){
        return std::to_string(val) + "=";
    }

    int string_to_int(const std::string_view &val, int &length){
        int result = 0;
        length = 0;        
        auto[last_ptr, err] = std::from_chars(val.data(), val.data() + val.size(), result);
        if(std::errc() == err){
            length = last_ptr - val.data();
            return result;
        }
        assert(false && "string_to_int: Unable to convert value to string!");
        return 0;
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

void BitMask::set(size_t index)noexcept{
    assert(index < 8*sizeof(mask_));
    mask_ |= BITS[index];
}

void BitMask::unset(size_t index)noexcept{
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

    auto[last_ptr, err] = std::from_chars(data_.data() + pos_, data_.data() + data_.size(), curr_tag_id_);
    if(std::errc() == err && '=' == last_ptr[0]){
        pos_ = last_ptr - data_.data() + 1;
        return curr_tag_id_;
    }
    curr_tag_id_ = 0;
    assert(false && "Invalid tag-value format, '=' is expected after tag_id!");
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

    size_t count = data_.size();
    int result = 0;
    auto[last_ptr, err] = std::from_chars(data_.data() + pos_, data_.data() + count, result);
    if(std::errc() == err && 1 == last_ptr[0]){
        ++last_ptr;
        pos_ = last_ptr - data_.data();
        return result;
    }
    error_ = "Invalid int tag-value format, SOH is not found in message!";    
    return 0;
}

MsgSerialised::MsgSerialised(size_t count, size_t start_pos): 
    buffer_(count, 1), pos_(start_pos), size_(count)
{}

void MsgSerialised::resize(size_t val){
    if(size_ - pos_ < val){
        size_ *= 2;
        buffer_.resize(size_, 1);
    }
}

void MsgSerialised::append(const std::string &val){
    auto val_len = val.size();
    resize(val_len);
    std::copy(val.begin(), val.end(), buffer_.begin() + pos_);
    pos_ += val_len;
}

void MsgSerialised::append(size_t val){
    resize(32);
    if(0 == val){
        buffer_[pos_++] = '0';
        ++pos_; // skip SOH
        return;
    }else{
        auto [last_ptr, err] = std::to_chars(buffer_.data() + pos_, buffer_.data() + size_, val);
        if(std::errc() == err){
            pos_ = last_ptr - buffer_.data() + 1;// skip SOH
            return;
        }
        assert(false && "MsgSerialised::append: Unable to convert size_t to string!");
    }
}

void MsgSerialised::append(const int &val){
    resize(32);
    if(0 == val){
        buffer_[pos_++] = '0';
        ++pos_;// skip SOH
        return;
    }else{
        auto [last_ptr, err] = std::to_chars(buffer_.data() + pos_, buffer_.data() + size_, val);
        if(std::errc() == err){
            pos_ = last_ptr - buffer_.data() + 1; // skip SOH
            return;
        }
        assert(false && "MsgSerialised::append: Unable to convert int to string!");
    }
}

void MsgSerialised::append(char val){
    resize(32);
    buffer_[pos_++] = val;
    ++pos_;// skip SOH
}

void MsgSerialised::append(const std::string_view &val){
    resize(val.size() + 1);
    std::copy(std::begin(val), std::end(val), buffer_.begin() + pos_);
    pos_ += val.size() + 1;// skip SOH
}

void MsgSerialised::append(const fix::FIXDouble &val){
    append(val.to_string());
}

void MsgSerialised::append(const fix::FIXString &val){
    append(val.to_string());
}

void MsgSerialised::append(const fix::FIXDate &val){
    append(val.to_string());
}

void MsgSerialised::append(const fix::FIXDatetime &val){
    append(val.to_string());
}

void MsgSerialised::append(const fix::FIXRawData &val){
    append(val.to_string());
}

std::string MsgSerialised::to_string()const{
    return std::string(buffer_.begin(), buffer_.begin() + pos_);
}
