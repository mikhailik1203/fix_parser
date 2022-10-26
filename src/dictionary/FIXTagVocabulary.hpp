#pragma once

#include "../FIXParserDefines.hpp"
#include "FIXTagDefinition.hpp"
#include "FIXBlockDefinition.hpp"

namespace fix{

    // Contrains relation between tags and groups, blocks or messages
    class FIXTagVocabulary{
    public:
        FIXTagVocabulary(FIXProtocol protocol, const TagDefinitionsT &tags);
        
        FIXProtocol protocol()const noexcept{return protocol_;}

        const TagDefinitionsT &tags()const noexcept{return tags_;}

        const FIXTagDefinition &tag_definition(tag_id_t tag_id)const noexcept;
        
        bool add_tag_location(tag_id_t tag, const FIXTagLocation &location);
    private:
        FIXProtocol protocol_ = FIXProtocol::invalid;
        TagDefinitionsT tags_;
    };
}