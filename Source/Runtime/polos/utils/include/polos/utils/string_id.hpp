///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_STRING_ID_HPP
#define POLOS_UTILS_STRING_ID_HPP

#include <cstdint>
#include <string_view>

namespace polos::utils
{

using string_id = std::int64_t;

constexpr auto StrHash64(std::string_view const t_str) -> string_id
{
    // FNV-1a 64-bit hash
    // This algorithm provides good distribution and fast updates for string data
    // Reference: http://www.isthe.com/chongo/tech/comp/fnv/
    constexpr std::uint64_t kFnvPrime  = 1099511628211ULL;// 2^40 + 2^8 + 0xb3
    constexpr std::uint64_t kFnvOffset = 14695981039346656037ULL;

    std::uint64_t hash = kFnvOffset;

    unsigned char cur_chr{0U};

    for (char const chr : t_str)
    {
        cur_chr = static_cast<unsigned char>(chr);
        hash ^= cur_chr;  // XOR the low 8 bits
        hash *= kFnvPrime;// Multiply by the FNV prime
    }

    // Cast to signed string_id - the bits remain unchanged
    return static_cast<string_id>(hash);
}

}// namespace polos::utils

consteval auto operator""_sid(char const* t_str, std::size_t t_size) -> polos::utils::string_id
{
    return polos::utils::StrHash64(std::string_view{t_str, t_size});
}

#endif// POLOS_UTILS_STRING_ID_HPP
