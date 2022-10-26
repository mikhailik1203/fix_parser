#include "FIXTagVocabulary.hpp"

using namespace fix;

FIXTagVocabulary::FIXTagVocabulary(FIXProtocol protocol, const TagDefinitionsT &tags): 
    protocol_(protocol), tags_(tags)
{}

const FIXTagDefinition &FIXTagVocabulary::tag_definition(tag_id_t tag_id)const noexcept{
    static FIXTagDefinition empty{0, FIXTagType::invalid};
    auto it = tags_.find(tag_id);
    if(tags_.end() != it)
        return it->second;
    return empty;
}

bool FIXTagVocabulary::add_tag_location(tag_id_t tag, const FIXTagLocation &location){
    auto it = tags_.find(tag);
    if(tags_.end() == it)
        return false;
    it->second.add_location(location);
    return true;
}
