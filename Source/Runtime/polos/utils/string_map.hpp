///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_STRING_MAP_HPP
#define POLOS_UTILS_STRING_MAP_HPP

#include "polos/polos_api.hpp"
#include "polos/utils/string_id.hpp"

#include <string_view>

namespace polos::utils
{

[[nodiscard]] POLOS_API auto RegisterString(std::string_view tString) -> string_id;
[[nodiscard]] POLOS_API auto GetStringFromID(string_id tID) -> std::string_view;

}// namespace polos::utils

#endif// POLOS_UTILS_STRING_MAP_HPP
