//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef LOGGING_LOGGING_MACROS_H
#define LOGGING_LOGGING_MACROS_H

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

#define LOG_VAR_IMPL(Str, Variable) LOG_POLOS_INFO(Str, __FILE__, __LINE__, QUILL_STRINGIFY(Variable), Variable)
#define LogVariable(Variable)       LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)

#endif//LOGGING_LOGGING_MACROS_H
