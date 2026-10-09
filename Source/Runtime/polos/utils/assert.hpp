///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_ASSERT_HPP
#define POLOS_UTILS_ASSERT_HPP

#include "polos/polos_api.hpp"

#include <cassert>
#include <format>
#include <source_location>
#include <string_view>

namespace polos::utils
{

[[noreturn]] POLOS_API auto AbortAssertion(
    char const*          tCondition,
    std::string_view     tReason,
    std::source_location tLocation = std::source_location::current()) -> void;

}// namespace polos::utils

#ifdef NDEBUG
#    define PL_ASSERT(Condition, Fmt, ...)  \
        ((Condition) ? static_cast<void>(0) \
                     : ::polos::utils::AbortAssertion(#Condition, std::format(Fmt __VA_OPT__(, ) __VA_ARGS__)))
#else
#    define PL_ASSERT(Condition, Fmt, ...) assert(Condition)
#endif

#endif// POLOS_UTILS_ASSERT_HPP
