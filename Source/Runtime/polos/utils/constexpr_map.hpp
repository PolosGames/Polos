///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_CONSTEXPR_MAP_HPP
#define POLOS_UTILS_CONSTEXPR_MAP_HPP

#include "polos/logging/log_macros.hpp"
#include "polos/utils/error_code.hpp"
#include "polos/utils/pair.hpp"
#include "polos/utils/utils_error_domain.hpp"

#include <array>
#include <cstdlib>

namespace polos::utils
{
template<typename Key, typename Value, std::size_t Size>
class ConstexprMap
{
public:
    constexpr explicit ConstexprMap(std::array<Pair<Key, Value>, Size> const& tArr)
        : data{tArr}
    {}

    [[nodiscard]] constexpr auto at(const Key& tKey) const -> Result<Value>
    {
        const auto itr = std::ranges::find_if(data, [&tKey](Pair<Key, Value> const& tVal) {
            return tVal.first == tKey;
        });

        if (itr == std::end(data))
        {
            // TODO(sorbatdev): When moving this out, return a result.
            LogError("Cannot find value in map");
            return ErrorType{UtilsErrc::kValueNotFoundInMap};
        }
        return itr->second;
    }

    std::array<Pair<Key, Value>, Size> data;
};

template<typename Key, typename Value, std::size_t Size>
ConstexprMap(std::array<Pair<Key, Value>, Size>) -> ConstexprMap<Key, Value, Size>;
}// namespace polos::utils

#endif// POLOS_UTILS_CONSTEXPR_MAP_HPP
