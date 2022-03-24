#include "polos_pch.h"

#include "type_util.h"

namespace polos
{
    int64 Hashit(char const* const str)
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

    int64 Hashit(std::string const& str)
    {
        return Hashit(str.c_str());
    }

    int64 Hashit(std::string&& str)
    {
        return Hashit(str.c_str());
    }
}