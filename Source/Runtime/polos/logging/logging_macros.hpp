//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef LOGGING_LOGGING_MACROS_H
#define LOGGING_LOGGING_MACROS_H

#include "polos/logging/logger.hpp"

#include <quill/LogMacros.h>

#define LOG_POLOS_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(::polos::logging::Logger::Instance().GetPolosLogger(), fmt, ##__VA_ARGS__)
#define LOG_POLLY_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(::polos::logging::Logger::Instance().GetPollyLogger(), fmt, ##__VA_ARGS__)
#define LOG_APP_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(::polos::logging::Logger::Instance().GetAppLogger(), fmt, ##__VA_ARGS__)

#define LOG_POLOS_ERROR(fmt, ...) QUILL_LOG_ERROR(::polos::logging::Logger::Instance().GetPolosLogger(), fmt, ##__VA_ARGS__)
#define LOG_POLLY_ERROR(fmt, ...) QUILL_LOG_ERROR(::polos::logging::Logger::Instance().GetPollyLogger(), fmt, ##__VA_ARGS__)
#define LOG_APP_ERROR(fmt, ...) QUILL_LOG_ERROR(::polos::logging::Logger::Instance().GetAppLogger(), fmt, ##__VA_ARGS__)

#define LOG_POLOS_WARN(fmt, ...) QUILL_LOG_WARNING(::polos::logging::Logger::Instance().GetPolosLogger(), fmt, ##__VA_ARGS__)
#define LOG_POLLY_WARN(fmt, ...) QUILL_LOG_WARNING(::polos::logging::Logger::Instance().GetPollyLogger(), fmt, ##__VA_ARGS__)
#define LOG_APP_WARN(fmt, ...) QUILL_LOG_WARNING(::polos::logging::Logger::Instance().GetAppLogger(), fmt, ##__VA_ARGS__)

#define LOG_POLOS_INFO(fmt, ...) QUILL_LOG_INFO(::polos::logging::Logger::Instance().GetPolosLogger(), fmt, ##__VA_ARGS__)
#define LOG_POLLY_INFO(fmt, ...) QUILL_LOG_INFO(::polos::logging::Logger::Instance().GetPollyLogger(), fmt, ##__VA_ARGS__)
#define LOG_APP_INFO(fmt, ...) QUILL_LOG_INFO(::polos::logging::Logger::Instance().GetAppLogger(), fmt, ##__VA_ARGS__)

#define LOG_POLOS_TRACE(fmt, ...) QUILL_LOG_TRACE_L1(::polos::logging::Logger::Instance().GetPolosLogger(), fmt, ##__VA_ARGS__)
#define LOG_POLLY_TRACE(fmt, ...) QUILL_LOG_TRACE_L1(::polos::logging::Logger::Instance().GetPollyLogger(), fmt, ##__VA_ARGS__)
#define LOG_APP_TRACE(fmt, ...) QUILL_LOG_TRACE_L1(::polos::logging::Logger::Instance().GetAppLogger(), fmt, ##__VA_ARGS__)

#define LOG_VAR_IMPL(Str, Variable)  LOG_POLOS_INFO(Str, __FILE__, __LINE__, QUILL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)            LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)

#endif //LOGGING_LOGGING_MACROS_H
