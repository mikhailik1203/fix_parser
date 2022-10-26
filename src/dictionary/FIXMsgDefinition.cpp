#include "FIXMsgDefinition.hpp"

#include <algorithm>

using namespace fix;

bool FIXMsgDefinition::support_tag(tag_id_t tag_id)const noexcept{
    auto it = std::find(tags_.begin(), tags_.end(), tag_id);
    return tags_.end() != it;
}

