///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/event.hpp"

#include <mutex>
#include <string>
#include <unordered_map>

namespace polos::communication::detail
{

static auto FnvHash(std::string const& t_str) -> std::int64_t
{
    constexpr std::int64_t k_FnvPrime{1099511628211LL};
    constexpr std::int64_t k_FnvOffset{static_cast<std::int64_t>(14695981039346656037ULL)};
    std::int64_t           hash = k_FnvOffset;
    for (char const c : t_str)
    {
        hash ^= c;
        hash *= k_FnvPrime;
    }
    return hash;
}

auto RetrieveEventHash(char const* t_name) -> std::int64_t
{
    static std::unordered_map<std::string, std::int64_t> hash_map;
    static std::mutex                                    mutex;

    std::lock_guard lock(mutex);
    auto const      it = hash_map.find(t_name);
    if (it == hash_map.end())
    {
        auto const hash_value = FnvHash(t_name);
        hash_map.emplace(t_name, hash_value);
        return hash_value;
    }
    return it->second;
}

}// namespace polos::communication::detail
