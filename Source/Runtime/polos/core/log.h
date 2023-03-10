#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

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
            k_LoggerNone
        };
    public:
        Log();
    public:
        void Startup();
        void Shutdown();

        template<typename... Args>
        void Critical(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args);

        template<typename... Args>
        void Error(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args);

        template<typename... Args>
        void Warn(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args);

        template<typename... Args>
        void Info(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args);

        template<typename... Args>
        void Trace(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args);

        static Log& Instance()
        {
            return *s_Instance;
        }
    private:
        static Log* s_Instance;
    private:
        std::array<SharedPtr<spdlog::logger>, k_LoggerNone> m_Loggers;
    };

    template<typename... Args>
    inline void Log::Critical(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args)
    {
        m_Loggers[p_Type]->critical(p_FmtStr, std::forward<Args>(p_Args)...);
    }

    template<typename... Args>
    inline void Log::Error(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args)
    {
        m_Loggers[p_Type]->error(p_FmtStr, std::forward<Args>(p_Args)...);
    }

    template<typename... Args>
    inline void Log::Warn(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args)
    {
        m_Loggers[p_Type]->warn(p_FmtStr, std::forward<Args>(p_Args)...);
    }

    template<typename... Args>
    inline void Log::Info(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args)
    {
        m_Loggers[p_Type]->info(p_FmtStr, std::forward<Args>(p_Args)...);
    }

    template<typename... Args>
    inline void Log::Trace(uint8 p_Type, spdlog::format_string_t<Args...> p_FmtStr, Args&&... p_Args)
    {
        m_Loggers[p_Type]->trace(p_FmtStr, std::forward<Args>(p_Args)...);
    }
} // namespace polos

#define LOG_ENGINE_CRITICAL(...) ::polos::Log::Instance().Critical(::polos::Log::logger_type::k_LoggerEngine, __VA_ARGS__)
#define LOG_EDITOR_CRITICAL(...) ::polos::Log::Instance().Critical(::polos::Log::logger_type::k_LoggerEditor, __VA_ARGS__)
#define LOG_CLIENT_CRITICAL(...) ::polos::Log::Instance().Critical(::polos::Log::logger_type::k_LoggerClient, __VA_ARGS__)

#define LOG_ENGINE_WARN(...) ::polos::Log::Instance().Warn(::polos::Log::logger_type::k_LoggerEngine, __VA_ARGS__)
#define LOG_EDITOR_WARN(...) ::polos::Log::Instance().Warn(::polos::Log::logger_type::k_LoggerEditor, __VA_ARGS__)
#define LOG_CLIENT_WARN(...) ::polos::Log::Instance().Warn(::polos::Log::logger_type::k_LoggerClient, __VA_ARGS__)

#define LOG_ENGINE_INFO(...) ::polos::Log::Instance().Info(::polos::Log::logger_type::k_LoggerEngine, __VA_ARGS__)
#define LOG_EDITOR_INFO(...) ::polos::Log::Instance().Info(::polos::Log::logger_type::k_LoggerEditor, __VA_ARGS__)
#define LOG_CLIENT_INFO(...) ::polos::Log::Instance().Info(::polos::Log::logger_type::k_LoggerClient, __VA_ARGS__)

#define LOG_ENGINE_ERROR(...) ::polos::Log::Instance().Error(::polos::Log::logger_type::k_LoggerEngine, __VA_ARGS__)
#define LOG_EDITOR_ERROR(...) ::polos::Log::Instance().Error(::polos::Log::logger_type::k_LoggerEditor, __VA_ARGS__)
#define LOG_CLIENT_ERROR(...) ::polos::Log::Instance().Error(::polos::Log::logger_type::k_LoggerClient, __VA_ARGS__)

#define LOG_ENGINE_TRACE(...) ::polos::Log::Instance().Trace(::polos::Log::logger_type::k_LoggerEngine, __VA_ARGS__)
#define LOG_EDITOR_TRACE(...) ::polos::Log::Instance().Trace(::polos::Log::logger_type::k_LoggerEditor, __VA_ARGS__)
#define LOG_CLIENT_TRACE(...) ::polos::Log::Instance().Trace(::polos::Log::logger_type::k_LoggerClient, __VA_ARGS__)

#define LOG_VAR_IMPL(Str, Variable)  LOG_ENGINE_INFO(Str, __FILE__, __LINE__, PL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)            LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)

// TODO: Make events register their fields to somewhere? This way we can log events.
// This macro accepts the instance of the event, not the class of it
#define LOG_EVENT(Event) \
    static_assert(!std::is_base_of<pl::Event, decltype(Event)>::value, "[Logger] The passed argument is not an event!");
    