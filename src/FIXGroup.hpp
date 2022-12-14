#pragma once

#include "FIXParserDefines.hpp"
#include "FIXGroupMetadata.hpp"
#include "FIXGroupEntry.hpp"

namespace fix{
    class FIXDictionary;
    class FIXGroup;

    struct FIXGroup{
    public:
        FIXGroup(const FIXGroupMetadata &meta, size_t count);
        ~FIXGroup() = default;
        FIXGroup(const FIXGroup &) = default;
        FIXGroup &operator=(const FIXGroup &) = delete;
        FIXGroup(FIXGroup &&) noexcept = default;
        FIXGroup &operator=(FIXGroup &&) noexcept = delete;

        tag_id_t leading_tag()const noexcept;
        tag_id_t start_entry_tag()const noexcept;

        entry_size_t size()const noexcept;
        const FIXGroupEntry &entry(entry_id_t id)const noexcept;
        FIXGroupEntry &entry(entry_id_t id) noexcept;
        FIXGroupEntry &add_entry();
        void set_size(entry_size_t count);
        void clear_entries() noexcept;

    public:
        FIXTagType tag_type(tag_id_t tag)const;

    public:
        // creates instance of group entry from parsed data
        static FIXGroup parse(const FIXDictionary &dict, MsgReceived &data);

        // serializes group entry data to the sequence of bytes
        void serialize(MsgSerialised &buffer)const;

    private:    
        const FIXGroupMetadata &meta_;

        using EntriesT = std::vector<FIXGroupEntry>;
        EntriesT entries_;
    };

}