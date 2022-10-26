#include "FIXBlockDefinition.hpp"

#include <algorithm>

using namespace fix;

void FIXBlockDefinition::add_location(const FIXTagLocation &val){
    block_locations_.insert(val);
}

bool FIXBlockDefinition::support_tag(tag_id_t tag_id)const noexcept{
    auto it = std::find(tags_.begin(), tags_.end(), tag_id);
    return tags_.end() != it;
}

