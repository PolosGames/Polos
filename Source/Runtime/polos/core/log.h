#pragma once

#include "quill/LogMacros.h"
#include "quill/Logger.h"

#include "polos/utils/macro_util.h"
#include "polos/containers/containers.h"
#include "polos/utils/alias.h"
#include "polos/utils/macro_util.h"

namespace polos
{
    class Log
    {
    public:
        enum logger_type : uint8
        {
            k_LoggerEngine,
            k_LoggerEditor,
            k_LoggerClient,
            k_MaxLoggers
        };
    public:
        Log();
    public:
        void Startup();
        void Shutdown();

        static quill::Logger* GetEngineLogger();
        static quill::Logger* GetEditorLogger();
        static quill::Logger* GetClientLogger();
    private:
        static Log* s_Instance;
        std::array<UniquePtr<quill::Logger>, k_MaxLoggers> m_Loggers;
    };
} // namespace polos

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

#define LOG_VAR_IMPL(Str, Variable)  LOG_ENGINE_INFO(Str, __FILE__, __LINE__, PL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)            LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)

// TODO: Make events register their fields to somewhere? This way we can log events.
// This macro accepts the instance of the event, not the class of it
#define LOG_EVENT(Event) \
    static_assert(!std::is_base_of<pl::Event, decltype(Event)>::value, "[Logger] The passed argument is not an event!");
    