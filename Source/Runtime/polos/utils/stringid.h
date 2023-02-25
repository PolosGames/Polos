#pragma once

#include "alias.h"
#include "polos/containers/containers.h"

namespace polos
{
    extern HashMap<StringId, std::string> g_StringTable;

    inline constexpr StringId hash_function(cstring str)
    {
        int64 p = 31;
        int64 m = 1'000'000'009;
        int64 power_of_p = 1;
        int64 hash_val = 0;
    
        // Loop to calculate the hash value
        // by iterating over the elements of string
        char c = 0;
        for (size_t i = 0; str[i] != '\0'; i++)
        {
            c = str[i];
            hash_val = (hash_val + static_cast<int64>(c) * power_of_p) % m;
            power_of_p = (power_of_p * p) % m;
        }
    
        return (hash_val % m + m) % m;
    }
    
    inline constexpr StringId get_string_id(cstring str)
    {
        StringId sid = hash_function(str);
        return sid;
    }
    
    StringId         get_string_id(std::string const& str);
    std::string_view get_string_from_id(StringId sid);

} // namespace polos

consteval polos::StringId operator""_sid(char const* str, std::size_t /**/) { return polos::hash_function(str); }
