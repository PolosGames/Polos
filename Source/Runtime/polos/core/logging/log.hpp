//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//


#ifndef CORE_LOGGING_LOG_H
#define CORE_LOGGING_LOG_H

#include "polos/core/engine/engine_subsystem.hpp"

#include <quill/LogMacros.h>
#include <quill/Logger.h>

namespace polos::logging
{
namespace detail
{
    enum logger_type : std::uint8_t
    {
        k_LoggerEngine,
        k_LoggerEditor,
        k_LoggerClient,
        k_MaxLoggers
    };
} // namespace detail

class LoggingSubsystem : public core::EngineSubsystem<LoggingSubsystem>
{

};

} // namespace polos::logging


#define LOG_ENGINE_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(::polos::Log::GetEngineLogger(), fmt, ##__VA_ARGS__)
#define LOG_EDITOR_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(::polos::Log::GetEditorLogger(), fmt, ##__VA_ARGS__)
#define LOG_CLIENT_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(::polos::Log::GetClientLogger(), fmt, ##__VA_ARGS__)

#define LOG_ENGINE_WARN(fmt, ...) QUILL_LOG_WARNING(::polos::Log::GetEngineLogger(), fmt, ##__VA_ARGS__)
#define LOG_EDITOR_WARN(fmt, ...) QUILL_LOG_WARNING(::polos::Log::GetEditorLogger(), fmt, ##__VA_ARGS__)
#define LOG_CLIENT_WARN(fmt, ...) QUILL_LOG_WARNING(::polos::Log::GetClientLogger(), fmt, ##__VA_ARGS__)

#define LOG_ENGINE_INFO(fmt, ...) QUILL_LOG_INFO(::polos::Log::GetEngineLogger(), fmt, ##__VA_ARGS__)
#define LOG_EDITOR_INFO(fmt, ...) QUILL_LOG_INFO(::polos::Log::GetEditorLogger(), fmt, ##__VA_ARGS__)
#define LOG_CLIENT_INFO(fmt, ...) QUILL_LOG_INFO(::polos::Log::GetClientLogger(), fmt, ##__VA_ARGS__)

#define LOG_ENGINE_ERROR(fmt, ...) QUILL_LOG_ERROR(::polos::Log::GetEngineLogger(), fmt, ##__VA_ARGS__)
#define LOG_EDITOR_ERROR(fmt, ...) QUILL_LOG_ERROR(::polos::Log::GetEditorLogger(), fmt, ##__VA_ARGS__)
#define LOG_CLIENT_ERROR(fmt, ...) QUILL_LOG_ERROR(::polos::Log::GetClientLogger(), fmt, ##__VA_ARGS__)

#define LOG_ENGINE_TRACE(fmt, ...) QUILL_LOG_TRACE_L1(::polos::Log::GetEngineLogger(), fmt, ##__VA_ARGS__)
#define LOG_EDITOR_TRACE(fmt, ...) QUILL_LOG_TRACE_L1(::polos::Log::GetEditorLogger(), fmt, ##__VA_ARGS__)
#define LOG_CLIENT_TRACE(fmt, ...) QUILL_LOG_TRACE_L1(::polos::Log::GetClientLogger(), fmt, ##__VA_ARGS__)

#define LOG_VAR_IMPL(Str, Variable)  LOG_ENGINE_INFO(Str, __FILE__, __LINE__, QUILL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)            LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)

#endif //CORE_LOGGING_LOG_H
