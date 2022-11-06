#include "FIXMessage.hpp"
#include "FIXGroup.hpp"
#include <cassert>

using namespace fix;

namespace{
    template<typename T, typename ContT>
    const T &get_value(const FIXMessageMetadata &meta, const ContT& cont, const fix::BitMask &mask, FIXTagType exp_type, tag_id_t tag){
        const auto &tag_meta = meta.tag_metadata(tag);
        if(!mask.is_set(tag_meta.tag_position_) || exp_type != tag_meta.type_ || cont.size() <= tag_meta.value_index_){
            static T empty;
            return empty;
        }
        return cont[tag_meta.value_index_];
    }

    template<typename T, typename ContT>
    const T &get_value(const FIXMessageMetadata &meta, const ContT& cont, const fix::BitMask &mask, FIXTagType exp_type1, FIXTagType exp_type2, tag_id_t tag){
        const auto &tag_meta = meta.tag_metadata(tag);
        if(!mask.is_set(tag_meta.tag_position_) || (exp_type1 != tag_meta.type_ && exp_type2 != tag_meta.type_) || cont.size() <= tag_meta.value_index_){
            static T empty;
            return empty;
        }
        return cont[tag_meta.value_index_];
    }

    template<typename T, typename ContT>
    bool set_value(const FIXMessageMetadata &meta, ContT& cont, fix::BitMask &mask, FIXTagType exp_type, tag_id_t tag, const T &val){
        const auto &tag_meta = meta.tag_metadata(tag);
        if(exp_type != tag_meta.type_ || cont.size() <= tag_meta.value_index_){
            return false;
        }
        mask.set(tag_meta.tag_position_);
        cont[tag_meta.value_index_] = val;
        return true;
    }

    template<typename T, typename ContT>
    bool set_value(const FIXMessageMetadata &meta, ContT& cont, fix::BitMask &mask, FIXTagType exp_type1, FIXTagType exp_type2, tag_id_t tag, const T &val){
        const auto &tag_meta = meta.tag_metadata(tag);
        if((exp_type1 != tag_meta.type_ && exp_type2 != tag_meta.type_) || cont.size() <= tag_meta.value_index_){
            return false;
        }
        mask.set(tag_meta.tag_position_);
        cont[tag_meta.value_index_] = val;
        return true;
    }

    template<typename T, typename ContT>
    bool set_value(const TagMetadata &tag_meta, ContT& cont, fix::BitMask &mask, tag_id_t tag, const T &val){
        if(cont.size() <= tag_meta.value_index_){
            return false;
        }
        mask.set(tag_meta.tag_position_);
        cont[tag_meta.value_index_] = val;
        return true;
    }
}


FIXMessage::FIXMessage(const FIXMessageMetadata &meta): meta_(meta){
    tag_char_values_.resize(meta_.tag_char_count());
    tag_int_values_.resize(meta_.tag_int_count());
    tag_double_values_.resize(meta_.tag_double_count());
    tag_string_values_.resize(meta_.tag_string_count());
    tag_date_values_.resize(meta_.tag_date_count());
    tag_datetime_values_.resize(meta_.tag_datetime_count());
    tag_rawdata_values_.resize(meta_.tag_rawdata_count());
    assert(meta_.tags_count() <= available_tags_.size());

    // set availability for all groups - they are available by default
    const auto &grp_metas = meta_.groups();
    for(const auto &grp_meta_pos: grp_metas){
        available_tags_.set(grp_meta_pos.second);
        tag_group_values_.emplace_back(grp_meta_pos.first, 0);
    }
}

FIXMessage::FIXMessage(FIXMessage &&val)noexcept : meta_(val.meta_){
    std::swap(tag_char_values_, val.tag_char_values_);
    std::swap(tag_int_values_, val.tag_int_values_);
    std::swap(tag_double_values_, val.tag_double_values_);
    std::swap(tag_string_values_, val.tag_string_values_);
    std::swap(tag_date_values_, val.tag_date_values_);
    std::swap(tag_datetime_values_, val.tag_datetime_values_);
    std::swap(tag_rawdata_values_, val.tag_rawdata_values_);
    std::swap(tag_group_values_, val.tag_group_values_);
    std::swap(available_tags_, val.available_tags_);
    std::swap(raw_msg_data_, val.raw_msg_data_);
}

FIXProtocol FIXMessage::protocol()const noexcept{
    return meta_.protocol();
}

const FIXMsgType &FIXMessage::message_type()const noexcept{
    return meta_.msg_type();
}

bool FIXMessage::has_tag(tag_id_t tag)const noexcept{
    const auto &tag_meta = meta_.tag_metadata(tag);
    return available_tags_.is_set(tag_meta.tag_position_);
}

bool FIXMessage::support_tag(tag_id_t tag)const noexcept{
    return meta_.support_tag(tag);
}

FIXTagType FIXMessage::tag_type(tag_id_t tag)const noexcept{
    return meta_.tag_type(tag);
}

bool FIXMessage::get_bool(tag_id_t tag)const noexcept{
    return 'Y' == get_value<char>(meta_, tag_char_values_, available_tags_, FIXTagType::BOOL, tag);
}

char FIXMessage::get_char(tag_id_t tag)const noexcept{
    return get_value<char>(meta_, tag_char_values_, available_tags_, FIXTagType::CHAR, tag);
}

int FIXMessage::get_int(tag_id_t tag)const noexcept{
    return get_value<int>(meta_, tag_int_values_, available_tags_, FIXTagType::INT, FIXTagType::RAWDATALEN, tag);
}

const FIXDouble &FIXMessage::get_double(tag_id_t tag)const noexcept{
    return get_value<FIXDouble>(meta_, tag_double_values_, available_tags_, FIXTagType::DOUBLE, tag);
}

const FIXString &FIXMessage::get_string(tag_id_t tag)const noexcept{
    return get_value<FIXString>(meta_, tag_string_values_, available_tags_, FIXTagType::STRING, tag);
}

const FIXDate &FIXMessage::get_date(tag_id_t tag)const noexcept{
    return get_value<FIXDate>(meta_, tag_date_values_, available_tags_, FIXTagType::DATE, tag);
}

const FIXDatetime &FIXMessage::get_datetime(tag_id_t tag)const noexcept{
    return get_value<FIXDatetime>(meta_, tag_datetime_values_, available_tags_, FIXTagType::DATETIME, tag);
}

const FIXRawData &FIXMessage::get_rawdata(tag_id_t tag)const noexcept{
    return get_value<FIXRawData>(meta_, tag_rawdata_values_, available_tags_, FIXTagType::RAWDATA, tag);
}

const FIXGroup &FIXMessage::get_group(tag_id_t tag)const noexcept{
    const auto &tag_meta = meta_.tag_metadata(tag);
    if(!available_tags_.is_set(tag_meta.tag_position_) || FIXTagType::GROUP != tag_meta.type_ || tag_group_values_.size() <= tag_meta.value_index_){
        static FIXGroup empty(DUMMY_GROUP_METADATA, 0);
        return empty;
    }
    return tag_group_values_[tag_meta.value_index_];
}

FIXGroup &FIXMessage::get_group(tag_id_t tag){
    const auto &tag_meta = meta_.tag_metadata(tag);
    if(FIXTagType::GROUP != tag_meta.type_ || tag_group_values_.size() <= tag_meta.value_index_){
        static FIXGroup empty(DUMMY_GROUP_METADATA, 0);
        return empty;
    }
    return tag_group_values_[tag_meta.value_index_];
}

bool FIXMessage::set_bool(tag_id_t tag, bool val){
    return ::set_value(meta_, tag_char_values_, available_tags_, FIXTagType::BOOL, tag, (val)?('Y'):('N'));
}

bool FIXMessage::set_char(tag_id_t tag, char val){
    return ::set_value(meta_, tag_char_values_, available_tags_, FIXTagType::CHAR, tag, val);
}

bool FIXMessage::set_int(tag_id_t tag, int val){
    return ::set_value(meta_, tag_int_values_, available_tags_, FIXTagType::INT, FIXTagType::RAWDATALEN, tag, val);
}

bool FIXMessage::set_double(tag_id_t tag, const FIXDouble &val){
    return ::set_value(meta_, tag_double_values_, available_tags_, FIXTagType::DOUBLE, tag, val);
}
bool FIXMessage::set_string(tag_id_t tag, const FIXString &val){
    return ::set_value(meta_, tag_string_values_, available_tags_, FIXTagType::STRING, tag, val);
}

bool FIXMessage::set_date(tag_id_t tag, const FIXDate &val){
    return ::set_value(meta_, tag_date_values_, available_tags_, FIXTagType::DATE, tag, val);
}

bool FIXMessage::set_datetime(tag_id_t tag, const FIXDatetime &val){
    return ::set_value(meta_, tag_datetime_values_, available_tags_, FIXTagType::DATETIME, tag, val);
}

bool FIXMessage::set_rawdata(tag_id_t tag, const FIXRawData &val){
    return ::set_value(meta_, tag_rawdata_values_, available_tags_, FIXTagType::RAWDATA, tag, val);
}

void FIXMessage::clear_tag(tag_id_t tag) noexcept{
    const auto &tag_meta = meta_.tag_metadata(tag);
    available_tags_.unset(tag_meta.tag_position_);
}


bool FIXMessage::set_value(const TagMetadata &tag_meta, tag_id_t tag, MsgReceived &data){
    switch(tag_meta.type_){
        case FIXTagType::BOOL:{
            bool v = data.parse_bool_value();
            return ::set_value(tag_meta, tag_char_values_, available_tags_, tag, (v)?('Y'):('N'));
        }   
        break;
        case FIXTagType::CHAR:{
            return ::set_value(tag_meta, tag_char_values_, available_tags_, tag, data.parse_char_value());
        }   
        break;
        case FIXTagType::RAWDATALEN:
        case FIXTagType::INT:{
            return ::set_value(tag_meta, tag_int_values_, available_tags_, tag, data.parse_int_value());
        }   
        break;
        case FIXTagType::STRING:{
            return ::set_value(tag_meta, tag_string_values_, available_tags_, tag, FIXString{data.parse_value()});
        }
        break;
        case FIXTagType::DOUBLE:{
            return ::set_value(tag_meta, tag_double_values_, available_tags_, tag, FIXDouble{data.parse_value()});
        }
        break;
        case FIXTagType::DATE:{
            return ::set_value(tag_meta, tag_date_values_, available_tags_, tag, FIXDate{data.parse_value()});
        }
        break;
        case FIXTagType::DATETIME:{
            return ::set_value(tag_meta, tag_datetime_values_, available_tags_, tag, FIXDatetime{data.parse_value()});
        }
        break;
        case FIXTagType::RAWDATA:{
            return ::set_value(tag_meta, tag_rawdata_values_, available_tags_, tag, FIXRawData{data.parse_value()});
        }
        break;
        case FIXTagType::GROUP:{
            assert(false && "FIXGroupEntry::set_value: not implemented for group!" );
            return false;
        }
        break;
        default:
            assert(false && "FIXGroupEntry::set_value: unknown tag's type!" );
            return false;
    };
    return true;
}

void FIXMessage::serialize(MsgSerialised &buffer)const{
    meta_.serialize(*this, buffer);
}
