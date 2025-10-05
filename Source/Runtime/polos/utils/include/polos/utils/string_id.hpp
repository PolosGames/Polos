//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_STRING_ID_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_STRING_ID_HPP_

#include <cstdint>
#include <string_view>

namespace polos::utils
{

inline constexpr std::int64_t StrHash64(std::string_view t_str)
{
    std::int64_t p          = 31;
    std::int64_t m          = 1'000'000'009;
    std::int64_t power_of_p = 1;
    std::int64_t hash_val   = 0;

    char c = 0;
    for (std::size_t i = 0; t_str[i] != '\0'; ++i)
    {
        c          = t_str[i];
        hash_val   = (hash_val + static_cast<std::int64_t>(c) * power_of_p) % m;
        power_of_p = (power_of_p * p) % m;
    }

    return (hash_val % m + m) % m;
}

inline constexpr std::int64_t GetStringId(std::string_view t_str)
{
    std::int64_t sid = StrHash64(t_str);
    return sid;
}

}// namespace polos::utils

consteval std::int64_t operator""_sid(char const* t_str, std::size_t t_size)
{
    return polos::utils::StrHash64(std::string_view{t_str, t_size});
}

#endif// POLOS_UTILS_INCLUDE_POLOS_UTILS_STRING_ID_HPP_
