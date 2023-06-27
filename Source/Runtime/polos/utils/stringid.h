#pragma once

#include "alias.h"
#include "polos/containers/containers.h"

namespace polos
{
    inline constexpr StringId StrHash64(cstring p_Str)
    {
        int64 p = 31;
        int64 m = 1'000'000'009;
        int64 power_of_p = 1;
        int64 hash_val = 0;
    
        // Loop to calculate the hash value
        // by iterating over the elements of string
        char c = 0;
        for (size_t i = 0; p_Str[i] != '\0'; ++i)
        {
            c          = p_Str[i];
            hash_val   = (hash_val + static_cast<int64>(c) * power_of_p) % m;
            power_of_p = (power_of_p * p) % m;
        }
    
        return (hash_val % m + m) % m;
    }

    uint32 StrHash32(cstring p_Str);
    
    inline constexpr StringId GetStringId(cstring p_Str)
    {
        StringId sid = StrHash64(p_Str);
        return sid;
    }
    
    StringId         GetStringId(std::string const& p_Str);
    std::string_view GetStringFromId(StringId p_Sid);

} // namespace polos

consteval polos::StringId operator""_sid(char const* p_Str, std::size_t /**/) { return polos::StrHash64(p_Str); }
