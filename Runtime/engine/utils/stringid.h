#ifndef POLOS_UTILS_STRINGID_H_
#define POLOS_UTILS_STRINGID_H_

#include "alias.h"
#include "containers/containers.h"

namespace polos
{
    static HashMap<StringId, std::string> g_StringTable;
    
    inline constexpr StringId hash_function(cstring str)
    {
        int32 p = 31;
        int32 m = 1'000'000'009;
        int64 power_of_p = 1;
        int64 hash_val = 0;
    
        // Loop to calculate the hash value
        // by iterating over the elements of string
        char c = 0;
        for (size_t i = 0; str[i] != '\0'; i++)
        {
            c = str[i];
            hash_val = (hash_val + (c - 'a' + 1) * power_of_p) % m;
            power_of_p = (power_of_p * p) % m;
        }
    
        return (hash_val % m + m) % m;
    }
    
    inline constexpr StringId get_string_id(cstring str)
    {
        StringId sid = hash_function(str);
        return sid;
    }
    
    inline StringId get_string_id(std::string const& str)
    {
        StringId sid = hash_function(str.c_str());
        
        if(!g_StringTable.contains(sid))
        {
            g_StringTable.insert({sid, str});
        }
        
        return sid;
    }
    
    constexpr StringId operator""_sid(cstring str, std::size_t _) { return hash_function(str); }
} // namespace polos

#endif //POLOSUTILS__STRINGID_H_
