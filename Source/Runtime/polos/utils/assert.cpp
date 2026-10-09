///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/utils/assert.hpp"

#include <cstdio>
#include <cstdlib>
#include <print>

namespace polos::utils
{

[[noreturn]] auto AbortAssertion(char const* tCondition, std::string_view tReason, std::source_location tLocation)
    -> void
{
    // Keep fatal diagnostics available before logging initializes or after it shuts down.
    std::println(stderr, "PL_ASSERT failed: {}", tCondition);
    std::println(stderr, "Reason: {}", tReason);
    std::println(stderr, "At {}:{} ({})", tLocation.file_name(), tLocation.line(), tLocation.function_name());
    std::ignore = std::fflush(stderr);
    std::abort();
}

}// namespace polos::utils
