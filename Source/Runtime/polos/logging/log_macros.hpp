///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_LOGGING_LOG_MACROS_HPP
#define POLOS_LOGGING_LOG_MACROS_HPP

#include "polos/logging/logger.hpp"
#include "polos/utils/macro_utilities.hpp"

#include <format>

#define MAKE_LOGGER_TYPE(Name) ::polos::logging::LogContext::PL_CONCAT(k, Name)

#if defined(PL_LOGGER_TYPE)
#    define LOGGER_TYPE MAKE_LOGGER_TYPE(PL_LOGGER_TYPE)
#else
#    error "PL_LOGGER_TYPE not defined"
#endif

#define PL_LOG(Context, Level, ...)                                                               \
    do                                                                                            \
    {                                                                                             \
        auto const     polosLogContext = (Context);                                               \
        constexpr auto polosLogLevel   = ::polos::logging::LogLevel::Level;                       \
        if (::polos::logging::ShouldLog(polosLogContext, polosLogLevel))                          \
        {                                                                                         \
            ::polos::logging::WriteLog(polosLogContext, polosLogLevel, std::format(__VA_ARGS__)); \
        }                                                                                         \
    } while (false)

#define LogCritical(...) PL_LOG(LOGGER_TYPE, kCritical, __VA_ARGS__)
#define LogError(...)    PL_LOG(LOGGER_TYPE, kError, __VA_ARGS__)
#define LogWarn(...)     PL_LOG(LOGGER_TYPE, kWarning, __VA_ARGS__)
#define LogInfo(...)     PL_LOG(LOGGER_TYPE, kInfo, __VA_ARGS__)
#define LogDebug(...)    PL_LOG(LOGGER_TYPE, kDebug, __VA_ARGS__)
#define LogTrace(...)    PL_LOG(LOGGER_TYPE, kTrace, __VA_ARGS__)

// Explicit contexts for functions defined in public headers.
#define LOG_CTX_POLOS ::polos::logging::LogContext::kPolos
#define LOG_CTX_POLLY ::polos::logging::LogContext::kPolly
#define LOG_CTX_APP   ::polos::logging::LogContext::kApp

#define LogCriticalCtx(Context, ...) PL_LOG(Context, kCritical, __VA_ARGS__)
#define LogErroCtx(Context, ...)     PL_LOG(Context, kError, __VA_ARGS__)
#define LogWarnCtx(Context, ...)     PL_LOG(Context, kWarning, __VA_ARGS__)
#define LogInfoCtx(Context, ...)     PL_LOG(Context, kInfo, __VA_ARGS__)
#define LogDebugCtx(Context, ...)    PL_LOG(Context, kDebug, __VA_ARGS__)
#define LogTraceCtx(Context, ...)    PL_LOG(Context, kTrace, __VA_ARGS__)

#define LogVariable(Variable) LogInfo("{} = {}", #Variable, Variable)

#endif// POLOS_LOGGING_LOG_MACROS_HPP
