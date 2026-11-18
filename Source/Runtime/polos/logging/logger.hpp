///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_LOGGING_LOGGER_HPP
#define POLOS_LOGGING_LOGGER_HPP

#include "polos/polos_api.hpp"

#include <cstdint>
#include <source_location>
#include <string_view>

namespace polos::logging
{

enum class LogContext : std::uint8_t
{
    kPolos,
    kPolly,
    kApp,
};

enum class LogLevel : std::uint8_t
{
    kTrace,
    kDebug,
    kInfo,
    kWarning,
    kError,
    kCritical,
};

[[nodiscard]] POLOS_API auto GetConsoleSinkName() -> std::string_view;
[[nodiscard]] POLOS_API auto ShouldLog(LogContext tContext, LogLevel tLevel) -> bool;
POLOS_API void               WriteLog(
    LogContext           tContext,
    LogLevel             tLevel,
    std::string_view     tMessage,
    std::source_location tLocation = std::source_location::current());
POLOS_API void FlushLogger(LogContext tContext);

}// namespace polos::logging

#endif// POLOS_LOGGING_LOGGER_HPP
