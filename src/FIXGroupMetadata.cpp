#include "FIXGroupMetadata.hpp"
#include "./dictionary/FIXDictionary.hpp"
#include "FIXGroup.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

using namespace fix;

FIXGroupMetadata::FIXGroupMetadata(tag_id_t leading_tag_id, const FIXDictionary &dict){
    if(0 == leading_tag_id)
        return;
    const auto &grps = dict.groups();
    auto grp_it = grps.find(leading_tag_id);
    if(grps.end() == grp_it)
        throw std::runtime_error("FIXGroupMetadata: Group with tag [" + std::to_string(leading_tag_id) + "] is not found in FIXDictionaty!" );

    leading_tag_id_ = leading_tag_id;
    entry_start_tag_id_ = grp_it->second.entry_tag();
    location_id_ = grp_it->second.location_id();
    leading_tag_prefix_ = fix::to_tag_prefix(leading_tag_id_);

    const auto &msg_tags = grp_it->second.tags();
    const auto &msg_blocks = grp_it->second.blocks();
    const auto &msg_groups = grp_it->second.groups();
    const auto &msg_positions = grp_it->second.positions();

    tag_metadata_.reserve(msg_tags.size());
    const auto &vocab = dict.vocabulary();
    for(auto [pos_type, pos_index]: msg_positions){
        switch(pos_type){
            case PositionType::tag:{
                assert(static_cast<size_t>(pos_index) < msg_tags.size());
                auto tag_id = msg_tags[pos_index];
                assert(0 < tag_id);
                process_tag(dict, vocab, tag_id);
            }
            break;
            case PositionType::block:{
                assert(static_cast<size_t>(pos_index) < msg_blocks.size());
                auto block_id = msg_blocks[pos_index];
                assert(0 < block_id);
                process_block(dict, vocab, block_id);
            }
            break;
            case PositionType::group:{
                assert(static_cast<size_t>(pos_index) < msg_groups.size());
                auto grp_tag_id = msg_groups[pos_index];
                assert(0 < grp_tag_id);
                size_t value_index = tag_group_values_.size();
                auto tag_position_idx = tag_metadata_.size();                
                tag_group_values_.push_back({FIXGroupMetadata(grp_tag_id, dict), tag_position_idx});
                tag_to_meta_idx_[grp_tag_id] = tag_position_idx;
                tag_metadata_.emplace_back(TagMetadata{grp_tag_id, FIXTagType::GROUP, value_index, tag_position_idx, ""});
            }
            break;
            default:
                throw std::runtime_error("FIXGroupMetadata: Unknown PositionType!");
        };
    }
}

bool FIXGroupMetadata::support_tag(tag_id_t tag)const noexcept{
    return tag_to_meta_idx_.end() != tag_to_meta_idx_.find(tag);
}

FIXTagType FIXGroupMetadata::tag_type(tag_id_t tag)const noexcept{
    auto it = tag_to_meta_idx_.find(tag);
    if(tag_to_meta_idx_.end() != it){
        return tag_metadata_[it->second].type_;
    }
    return FIXTagType::invalid;
}

const TagMetadata &FIXGroupMetadata::tag_metadata(tag_id_t tag)const noexcept{
    auto it = tag_to_meta_idx_.find(tag);
    if(tag_to_meta_idx_.end() != it){
        return tag_metadata_[it->second];
    }
    static TagMetadata empty;
    return empty;
}

void FIXGroupMetadata::process_tag(const FIXDictionary &dict, const FIXTagVocabulary &vocab, tag_id_t tag_id){
    const auto &tag_def = vocab.tag_definition(tag_id);

    auto tag_type = tag_def.tag_type();
    size_t value_index = 0;
    switch(tag_type){
        case FIXTagType::BOOL:
        case FIXTagType::CHAR:
            value_index = tag_char_count_++;
        break;
        case FIXTagType::RAWDATALEN:
        case FIXTagType::INT:
            value_index = tag_int_count_++;
        break;
        case FIXTagType::DOUBLE:
            value_index = tag_double_count_++;
        break;
        case FIXTagType::STRING:
            value_index = tag_string_count_++;
        break;
        case FIXTagType::DATE:
            value_index = tag_date_count_++;
        break;
        case FIXTagType::DATETIME:
            value_index = tag_datatime_count_++;
        break;
        case FIXTagType::RAWDATA:
            value_index = tag_rawdata_count_++;
        break;
        case FIXTagType::GROUP:
            value_index = tag_group_values_.size();
            tag_group_values_.emplace_back(FIXGroupMetadata(tag_id, dict), tag_metadata_.size());
        break;
        default:
            assert(false && "FIXGroupMetadata::process_tag: Tag's type is not supported!");
            return;
    }
    auto tag_position_idx = tag_metadata_.size();        
    tag_to_meta_idx_[tag_id] = tag_position_idx;
    std::string tag_prefix = std::to_string(tag_id) + "=";
    tag_metadata_.emplace_back(TagMetadata{tag_id, tag_type, value_index, tag_position_idx, tag_prefix});
}

void FIXGroupMetadata::process_block(const FIXDictionary &dict, const FIXTagVocabulary &vocab, block_id_t block_id)
{
    const auto &block_defs = dict.blocks();
    auto block_it = block_defs.find(block_id);
    if(block_defs.end() == block_it)
        throw std::runtime_error("FIXGroupMetadata::process_block: Unable to find block by id [" + std::to_string(block_id) + "]!" );

    const auto &blk_tags = block_it->second.tags();
    const auto &blk_positions = block_it->second.positions();
    for(auto [pos_type, pos_index]: blk_positions){
        switch(pos_type){
            case PositionType::tag:{
                assert(static_cast<size_t>(pos_index) < blk_tags.size());
                auto tag_id = blk_tags[pos_index];
                assert(0 < tag_id);
                process_tag(dict, vocab, tag_id);
            }
            break;
            case PositionType::block:{
                const auto &blk_blocks = block_it->second.blocks();
                assert(static_cast<size_t>(pos_index) < blk_blocks.size());
                auto block_id = blk_blocks[pos_index];
                assert(0 < block_id);
                process_block(dict, vocab, block_id);
            }
            break;
            case PositionType::group:{
                const auto &blk_groups = block_it->second.groups();
                assert(static_cast<size_t>(pos_index) < blk_groups.size());
                auto grp_tag_id = blk_groups[pos_index];
                assert(0 < grp_tag_id);
                size_t value_index = tag_group_values_.size();
                auto tag_position_idx = tag_metadata_.size();                        
                tag_group_values_.push_back({FIXGroupMetadata(grp_tag_id, dict), tag_position_idx});
                tag_to_meta_idx_[grp_tag_id] = tag_position_idx;
                tag_metadata_.emplace_back(TagMetadata{grp_tag_id, FIXTagType::GROUP, value_index, tag_position_idx, ""});
            }
            break;
            default:
                assert( false && "FIXGroupMetadata::process_block: Unknown PositionType!");
        };
    }
}

FIXGroup FIXGroupMetadata::create()const{
    FIXGroup grp(*this, 0);
    return grp;
}

void FIXGroupMetadata::serialize_group_size(size_t count,  MsgSerialised &buffer)const{
    buffer.append(leading_tag_prefix_);
    buffer.append(count);
}

void FIXGroupMetadata::serialize(const FIXGroupEntry &data, MsgSerialised &buffer)const{
    for(const auto &tag_meta: tag_metadata_){
        if(!data.available_tags_.is_set(tag_meta.tag_position_)){
            continue;
        }
        buffer.append(tag_meta.tag_prefix_);
        switch(tag_meta.type_){
            case FIXTagType::BOOL:{
                const auto &val = data.tag_char_values_[tag_meta.value_index_];   
                buffer.append(val);             
            }
            break;
            case FIXTagType::RAWDATALEN:
            case FIXTagType::INT:{
                const auto &val = data.tag_int_values_[tag_meta.value_index_];
                buffer.append(val);
            }   
            break;
            case FIXTagType::DOUBLE:{
                const auto &val = data.tag_double_values_[tag_meta.value_index_];
                buffer.append(val);
                /*const auto &str_val = val.to_string();
                buffer.insert(buffer.end(), str_val.begin(), str_val.end());
                buffer.push_back(1);*/
            }
            break;
            case FIXTagType::STRING:{
                const auto &val = data.tag_string_values_[tag_meta.value_index_];                
                buffer.append(val);
                /*const auto &str_val = val.to_string();
                buffer.insert(buffer.end(), str_val.begin(), str_val.end());
                buffer.push_back(1);*/
            }
            break;
            case FIXTagType::DATE:{
                const auto &val = data.tag_date_values_[tag_meta.value_index_];                
                buffer.append(val);
                /*const auto &str_val = val.to_string();
                buffer.insert(buffer.end(), str_val.begin(), str_val.end());
                buffer.push_back(1);*/
            }
            break;
            case FIXTagType::DATETIME:{
                const auto &val = data.tag_datetime_values_[tag_meta.value_index_];
                buffer.append(val);
                /*const auto &str_val = val.to_string();
                buffer.insert(buffer.end(), str_val.begin(), str_val.end());
                buffer.push_back(1);*/
            }
            break;
            case FIXTagType::RAWDATA:{
                const auto &val = data.tag_rawdata_values_[tag_meta.value_index_];
                buffer.append(val);
                /*const auto &str_val = val.to_string();
                buffer.insert(buffer.end(), str_val.begin(), str_val.end());
                buffer.push_back(1);*/
            }
            break;
            case FIXTagType::GROUP:{
                const auto &val = data.tag_group_values_[tag_meta.value_index_];
                if(0 < val.size()){
                    val.serialize(buffer);
                }
            }
            break;
            default:
                assert(false && "FIXGroupMetadata::serialize: Tag's type is not supported!" );
        };
    }
}

bool FIXGroupMetadata::set_tag_value(const fix::TagMetadata &meta_data, size_t &tag_id, 
                                     MsgReceived &data, FIXGroupEntry &entry, FIXGroup &grp)const{
    if(FIXTagType::GROUP != meta_data.type_){
        entry.set_value(meta_data, tag_id, data);
        if(!data.error_.empty())
            return false;
        if(FIXTagType::RAWDATALEN == meta_data.type_){
            auto raw_tag_id = data.current_tag_id();
            if(0 == raw_tag_id || !data.error_.empty())
                return false;
            if(entry_start_tag_id_ != raw_tag_id){
                auto meta_it = tag_to_meta_idx_.find(raw_tag_id);
                if(tag_to_meta_idx_.end() == meta_it){
                    return false; // tag_id is not related to this group - finished parsing this group
                }
                const auto &meta_data = tag_metadata_[meta_it->second];
                if(FIXTagType::RAWDATA != meta_data.type_){
                    data.error_ = "Next tag after RAWDATALEN is not RAWDATA!";
                    return false; // next tag should be raw data!
                }
                auto raw_val = data.parse_rawdata_value(entry.get_int(tag_id));
                if(!data.error_.empty())
                    return false;
                entry.set_rawdata(raw_tag_id, FIXRawData{raw_val});
                tag_id = raw_tag_id;
            }
        }
    }else{
        int grp_count = data.parse_int_value();
        if(0 == grp_count || !data.error_.empty())
            return false;
        const auto &grp_meta = tag_group_values_[meta_data.value_index_];
        grp_meta.first.parse(data, entry.get_group(tag_id), grp_count);
    }
    return true;
}

bool FIXGroupMetadata::process_entry(size_t &tag_id, MsgReceived &data, FIXGroupEntry &entry, FIXGroup &grp)const{
    do{
        tag_id = data.current_tag_id();
        if(0 == tag_id || !data.error_.empty())
            return false;
        if(entry_start_tag_id_ != tag_id){
            auto meta_it = tag_to_meta_idx_.find(tag_id);
            if(tag_to_meta_idx_.end() == meta_it){
                return false; // tag_id is not related to this group - finished parsing this group
            }
            const auto &meta_data = tag_metadata_[meta_it->second];
            if (!set_tag_value(meta_data, tag_id, data, entry, grp)){
                return false;
            }
        }
    }while (entry_start_tag_id_ != tag_id);
    return true;
}


FIXGroup FIXGroupMetadata::parse(MsgReceived &data)const{
    auto lead_tag = data.current_tag_id();
    if(leading_tag_id_ != lead_tag)
        return FIXGroup(*this, 0);
    size_t entry_count = data.parse_int_value();
    FIXGroup grp(*this, entry_count);
    for(size_t i = 0; i < entry_count; ++i){
        size_t tag_id = data.current_tag_id();
        if(0 == tag_id || !data.error_.empty())
            return grp;
        if(entry_start_tag_id_ != tag_id){
            std::stringstream ss;
            ss << "Invalid entry tag [" << tag_id << "] of the group [" 
                << leading_tag_id_ << "], unable to parse message!";
            data.error_ = ss.str();
            return grp;
        }
        auto meta_it = tag_to_meta_idx_.find(tag_id);
        if(tag_to_meta_idx_.end() == meta_it){
            return grp; // tag_id is not related to this group - finished parsing this group
        }
        const auto &meta_data = tag_metadata_[meta_it->second];

        auto &entry = grp.entry(i);
        entry.set_value(meta_data, tag_id, data);
        if(!data.error_.empty())
            return grp;

        if(!process_entry(tag_id, data, entry, grp)){
            return grp;
        }
    }
    return grp;
}

bool FIXGroupMetadata::parse(MsgReceived &data, FIXGroup &grp, size_t entry_count)const{
    grp.set_size(entry_count);
    for(size_t i = 0; i < entry_count; ++i){
        size_t tag_id = data.current_tag_id();
        if(0 == tag_id || !data.error_.empty())
            return false;
        if(entry_start_tag_id_ != tag_id){
            std::stringstream ss;
            ss << "Invalid entry tag [" << tag_id << "] of the group [" 
                << leading_tag_id_ << "], unable to parse message!";
            data.error_ = ss.str();
            return false;
        }
        auto meta_it = tag_to_meta_idx_.find(tag_id);
        if(tag_to_meta_idx_.end() == meta_it){
            return true; // tag_id is not related to this group - finished parsing this group
        }
        const auto &meta_data = tag_metadata_[meta_it->second];

        auto &entry = grp.entry(i);
        entry.set_value(meta_data, tag_id, data);
        if(!data.error_.empty())
            return false;
        if(!process_entry(tag_id, data, entry, grp)){
            return true;
        }
    }
    return false;
}

std::vector<FIXGroup> FIXGroupMetadata::create_nested()const{
    std::vector<FIXGroup> groups;
    groups.reserve(tag_group_values_.size());
    for(const auto &grp_meta: tag_group_values_){
        groups.emplace_back(grp_meta.first, 0);
    }
    return groups;
}