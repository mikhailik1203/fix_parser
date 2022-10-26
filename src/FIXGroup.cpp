#include "FIXGroup.hpp"
#include "./dictionary/FIXDictionary.hpp"

using namespace fix;

const size_t RESERVE_GROUP_SIZE = 8;

FIXGroup::FIXGroup(const FIXGroupMetadata &meta, size_t count): meta_(meta){
    entries_.reserve((0 == count)?(RESERVE_GROUP_SIZE):(count));
    for(size_t i = 0; i < count; ++i){
        entries_.emplace_back(meta_);
    }
}

tag_id_t FIXGroup::leading_tag()const{
    return meta_.leading_tag();
}

tag_id_t FIXGroup::start_entry_tag()const{
    return meta_.entry_tag();
}

entry_size_t FIXGroup::size()const{
    return entries_.size();
}

const FIXGroupEntry &FIXGroup::entry(entry_id_t idx)const{
    if(idx < entries_.size())
        return entries_[idx];
    throw std::runtime_error("Unable to get const_entry by index - no such index!");
}

FIXGroupEntry &FIXGroup::entry(entry_id_t idx){
    if(idx < entries_.size())
        return entries_[idx];
    throw std::runtime_error("Unable to get entry by index - no such index!");
}

FIXGroupEntry &FIXGroup::add_entry(){
    entries_.emplace_back(meta_);
    return entries_.back();
}

void FIXGroup::clear_entries(){
    entries_.clear();
    entries_.reserve(RESERVE_GROUP_SIZE);
}

FIXTagType FIXGroup::tag_type(tag_id_t tag)const{
    return meta_.tag_type(tag);
}

void FIXGroup::serialize(std::vector<char> &buffer)const{
    if(entries_.empty())
        return;
    meta_.serialize_group_size(entries_.size(), buffer);
    for(const auto &entry: entries_){
        entry.serialize(buffer);
    }
}

