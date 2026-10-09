///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_LOGGING_LOG_MACROS_HPP
#define POLOS_LOGGING_LOG_MACROS_HPP

#include "polos/logging/logger.hpp"
#include "polos/utils/macro_utilities.hpp"

#include <quill/LogMacros.h>

#include <source_location>

#define MAKE_LOGGER_TYPE(Name) PL_CONCAT(::polos::logging::Logger::Instance().Get, PL_CONCAT(Name, Logger))

#if defined(PL_LOGGER_TYPE)
#    define LOGGER_TYPE MAKE_LOGGER_TYPE(PL_LOGGER_TYPE)()
#elif
#    error "PL_LOGGER_TYPE not defined"
#endif// defined(PL_LOGGER_TYPE)

#define LogCritical(fmt, ...) QUILL_LOG_CRITICAL(LOGGER_TYPE, fmt, ##__VA_ARGS__)
#define LogError(fmt, ...)    QUILL_LOG_ERROR(LOGGER_TYPE, fmt, ##__VA_ARGS__)
#define LogWarn(fmt, ...)     QUILL_LOG_WARNING(LOGGER_TYPE, fmt, ##__VA_ARGS__)
#define LogInfo(fmt, ...)     QUILL_LOG_INFO(LOGGER_TYPE, fmt, ##__VA_ARGS__)
#define LogDebug(fmt, ...)    QUILL_LOG_DEBUG(LOGGER_TYPE, fmt, ##__VA_ARGS__)
#define LogTrace(fmt, ...)    QUILL_LOG_TRACE_L1(LOGGER_TYPE, fmt, ##__VA_ARGS__)

// Logging macros with a explicit context for inline, constexpr, template functions in header files
#define LOG_CTX_POLOS ::polos::logging::Logger::Instance().GetPolosLogger()
#define LOG_CTX_POLLY ::polos::logging::Logger::Instance().GetPollyLogger()
#define LOG_CTX_APP   ::polos::logging::Logger::Instance().GetAppLogger()

#define LogCriticalCtx(LoggerType, fmt, ...) QUILL_LOG_CRITICAL(LoggerType, fmt, ##__VA_ARGS__)
#define LogErroCtx(LoggerType, fmt, ...)     QUILL_LOG_ERROR(LoggerType, fmt, ##__VA_ARGS__)
#define LogWarnCtx(LoggerType, fmt, ...)     QUILL_LOG_WARNING(LoggerType, fmt, ##__VA_ARGS__)
#define LogInfoCtx(LoggerType, fmt, ...)     QUILL_LOG_INFO(LoggerType, fmt, ##__VA_ARGS__)
#define LogDebugCtx(LoggerType, fmt, ...)    QUILL_LOG_DEBUG(LoggerType, fmt, ##__VA_ARGS__)
#define LogTraceCtx(LoggerType, fmt, ...)    QUILL_LOG_TRACE_L1(LoggerType, fmt, ##__VA_ARGS__)

#define LOG_VAR_IMPL(Str, Variable) LOG_POLOS_INFO(Str, __FILE__, __LINE__, QUILL_STRINGIFY(Variable), Variable)
#define LogVariable(Variable)       LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)


#endif// POLOS_LOGGING_LOG_MACROS_HPP
