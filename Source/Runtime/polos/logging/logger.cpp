///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/logging/logger.hpp"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

#include <array>
#include <string>
#include <utility>

namespace polos::logging
{
namespace
{

auto GetLogger(LogContext tContext) -> quill::Logger*
{
    static auto const loggers = [] {
#if defined(NDEBUG)
        constexpr auto level = quill::LogLevel::Info;
#else
        constexpr auto level = quill::LogLevel::Debug;
#endif
        quill::Backend::start();
        auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(std::string(GetConsoleSinkName()));
        quill::PatternFormatterOptions const formatter{
            "%(log_level:<3): %(logger:<6) [%(short_source_location)] %(message)",
            "%H:%M:%S.%Qms",
        };
        std::array const contexts{
            quill::Frontend::create_or_get_logger("POLOS", sink, formatter),
            quill::Frontend::create_or_get_logger("POLLY", sink, formatter),
            quill::Frontend::create_or_get_logger("APP", sink, formatter),
        };
        for (auto* logger : contexts)
        {
            logger->set_log_level(level);
        }
        return contexts;
    }();
    return loggers[std::to_underlying(tContext)];
}

auto ToQuillLevel(LogLevel tLevel) -> quill::LogLevel
{
    constexpr std::array levels{
        quill::LogLevel::TraceL1,
        quill::LogLevel::Debug,
        quill::LogLevel::Info,
        quill::LogLevel::Warning,
        quill::LogLevel::Error,
        quill::LogLevel::Critical,
    };
    return levels[std::to_underlying(tLevel)];
}

}// namespace

auto GetConsoleSinkName() -> std::string_view
{
    return "pl_std_sink";
}

auto ShouldLog(LogContext tContext, LogLevel tLevel) -> bool
{
    return GetLogger(tContext)->should_log_statement(ToQuillLevel(tLevel));
}

void WriteLog(LogContext tContext, LogLevel tLevel, std::string_view tMessage, std::source_location tLocation)
{
    auto*      logger = GetLogger(tContext);
    auto const level  = ToQuillLevel(tLevel);
    QUILL_LOG_RUNTIME_METADATA(
        logger, level, tLocation.file_name(), tLocation.line(), tLocation.function_name(), "{}", tMessage);
}

void FlushLogger(LogContext tContext)
{
    GetLogger(tContext)->flush_log();
}

}// namespace polos::logging
