#include "FIXGroup.hpp"
#include "./dictionary/FIXDictionary.hpp"
#include <cassert>

using namespace fix;

const size_t RESERVE_GROUP_SIZE = 8;

FIXGroup::FIXGroup(const FIXGroupMetadata &meta, size_t count): meta_(meta){
    entries_.reserve((0 == count)?(RESERVE_GROUP_SIZE):(count));
    for(size_t i = 0; i < count; ++i){
        entries_.emplace_back(meta_);
    }
}

tag_id_t FIXGroup::leading_tag()const noexcept{
    return meta_.leading_tag();
}

tag_id_t FIXGroup::start_entry_tag()const noexcept{
    return meta_.entry_tag();
}

entry_size_t FIXGroup::size()const noexcept{
    return entries_.size();
}

const FIXGroupEntry &FIXGroup::entry(entry_id_t idx)const noexcept{
    if(idx < entries_.size())
        return entries_[idx];
    assert(false && "Unable to get const_entry by index - no such index!");
    return DUMMY_GROUP_ENTRY;
}

FIXGroupEntry &FIXGroup::entry(entry_id_t idx)noexcept{
    if(idx < entries_.size())
        return entries_[idx];
    assert(false && "Unable to get const_entry by index - no such index!");
    return DUMMY_GROUP_ENTRY;
}

FIXGroupEntry &FIXGroup::add_entry(){
    entries_.emplace_back(meta_);
    return entries_.back();
}

void FIXGroup::set_size(entry_size_t count){
    for(size_t i = entries_.size(); i < count; ++i){
        entries_.emplace_back(meta_);
    }
}

void FIXGroup::clear_entries() noexcept{
    entries_.clear();
    entries_.reserve(RESERVE_GROUP_SIZE);
}

FIXTagType FIXGroup::tag_type(tag_id_t tag)const{
    return meta_.tag_type(tag);
}

void FIXGroup::serialize(MsgSerialised &buffer)const{
    if(entries_.empty())
        return;
    meta_.serialize_group_size(entries_.size(), buffer);
    for(const auto &entry: entries_){
        entry.serialize(buffer);
    }
}

