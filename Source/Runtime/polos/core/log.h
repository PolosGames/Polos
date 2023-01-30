#pragma once
#ifndef POLOS_CORE_LOG_H_
#define POLOS_CORE_LOG_H_

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#include "polos/utils/macro_util.h"
#include "polos/containers/containers.h"
#include "polos/utils/alias.h"

namespace polos
{
    class Log
    {
    public:
        enum logger_type : uint8
        {
            kLoggerEngine,
            kLoggerEditor,
            kLoggerClient,
        
            kMaxLoggerType
        };
    public:
        void Startup();
        void Shutdown();

        template<typename ...Args>
        void critical(uint8 type, spdlog::format_string_t<Args...> fmt, Args &&... args);

        template<typename ...Args>
        void error(uint8 type, spdlog::format_string_t<Args...> fmt, Args &&... args);

        template<typename ...Args>
        void warn(uint8 type, spdlog::format_string_t<Args...> fmt, Args &&... args);

        template<typename ...Args>
        void info(uint8 type, spdlog::format_string_t<Args...> fmt, Args &&... args);

        template<typename ...Args>
        void trace(uint8 type, spdlog::format_string_t<Args...> fmt, Args &&... args);

        static Log& Instance()
        {
            return *s_Instance;
        }
    private:
        static Log* s_Instance;
    private:
        std::array<SharedPtr<spdlog::logger>, kMaxLoggerType> m_Logger;
    };
} // namespace polos

#include "log.inl"

#define LOG_ENGINE_CRITICAL(...) ::polos::Log::Instance().critical(::polos::Log::logger_type::kLoggerEngine, __VA_ARGS__)
#define LOG_ENGINE_ERROR(...)    ::polos::Log::Instance().error(   ::polos::Log::logger_type::kLoggerEngine, __VA_ARGS__)
#define LOG_ENGINE_WARN(...)     ::polos::Log::Instance().warn(    ::polos::Log::logger_type::kLoggerEngine, __VA_ARGS__)
#define LOG_ENGINE_INFO(...)     ::polos::Log::Instance().info(    ::polos::Log::logger_type::kLoggerEngine, __VA_ARGS__)
#define LOG_ENGINE_TRACE(...)    ::polos::Log::Instance().trace(   ::polos::Log::logger_type::kLoggerEngine, __VA_ARGS__)

#define LOG_EDITOR_CRITICAL(...) ::polos::Log::Instance().critical(::polos::Log::logger_type::kLoggerEditor, __VA_ARGS__)
#define LOG_EDITOR_ERROR(...)    ::polos::Log::Instance().error(   ::polos::Log::logger_type::kLoggerEditor, __VA_ARGS__)
#define LOG_EDITOR_WARN(...)     ::polos::Log::Instance().warn(    ::polos::Log::logger_type::kLoggerEditor, __VA_ARGS__)
#define LOG_EDITOR_INFO(...)     ::polos::Log::Instance().info(    ::polos::Log::logger_type::kLoggerEditor, __VA_ARGS__)
#define LOG_EDITOR_TRACE(...)    ::polos::Log::Instance().trace(   ::polos::Log::logger_type::kLoggerEditor, __VA_ARGS__)

#define LOG_CLIENT_CRITICAL(...) ::polos::Log::Instance().critical(::polos::Log::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_CLIENT_ERROR(...)    ::polos::Log::Instance().error(   ::polos::Log::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_CLIENT_WARN(...)     ::polos::Log::Instance().warn(    ::polos::Log::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_CLIENT_INFO(...)     ::polos::Log::Instance().info(    ::polos::Log::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_CLIENT_TRACE(...)    ::polos::Log::Instance().trace(   ::polos::Log::logger_type::kLoggerClient, __VA_ARGS__)

#include "polos/utils/macro_util.h"

#define LOG_VAR_IMPL(Str, Variable)  LOG_ENGINE_INFO(Str, __FILE__, __LINE__, PL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)            LOG_VAR_IMPL("\n  File: {0},\n  Line: {1},\n  value of {2} = {3}", Variable)

// TODO: Make events register their fields to somewhere? This way we can log
// events.
// This macro accepts the instance of the event, not the class of it
#define LOG_EVENT(Event) \
    static_assert(!std::is_base_of<pl::Event, decltype(Event)>::value, "[Logger] The passed argument is not an event!");

#endif /* POLOS_CORE_LOG_H_ */
