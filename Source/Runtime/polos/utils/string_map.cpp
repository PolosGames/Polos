///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/utils/string_map.hpp"

#include <string>
#include <unordered_map>

namespace polos::utils
{
namespace
{

auto Strings() -> std::unordered_map<string_id, std::string>&
{
    static std::unordered_map<string_id, std::string> strings;
    return strings;
}

}// namespace

auto RegisterString(std::string_view tString) -> string_id
{
    auto const strid = StrHash64(tString);
    Strings().try_emplace(strid, tString);
    return strid;
}

auto GetStringFromID(string_id tID) -> std::string_view
{
    auto const& strings = Strings();
    auto const  found   = strings.find(tID);
    return found != strings.end() ? std::string_view{found->second} : std::string_view{};
}

}// namespace polos::utils
