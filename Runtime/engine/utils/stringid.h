#ifndef POLOS_UTILS_STRINGID_H_
#define POLOS_UTILS_STRINGID_H_

#include "alias.h"

namespace polos
{
    inline constexpr StringId get_string_id(cstring str)
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
    
    inline constexpr StringId get_string_id(std::string const& str)
    {
        return get_string_id(str.c_str());
    }
    
    inline constexpr StringId get_string_id(std::string_view str)
    {
        return get_string_id(str.data());
    }
    
    constexpr StringId operator""_sid(char const* str, std::size_t _) { return get_string_id(str); }
} // namespace polos

#endif //POLOSUTILS__STRINGID_H_
