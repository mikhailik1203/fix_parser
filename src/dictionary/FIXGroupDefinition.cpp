#include "FIXGroupDefinition.hpp"

#include <algorithm>

using namespace fix;

void FIXGroupDefinition::add_location(const FIXTagLocation &val){
    group_locations_.insert(val);
}

bool FIXGroupDefinition::support_tag(tag_id_t tag_id)const noexcept{
    auto it = std::find(tags_.begin(), tags_.end(), tag_id);
    return tags_.end() != it;
}

