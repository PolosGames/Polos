#pragma once
#ifndef POLOS_UTILS_LOG_H_
#define POLOS_UTILS_LOG_H_

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#include "utils/macro_util.h"
#include "containers/containers.h"
#include "utils/alias.h"

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
        Log() noexcept = default;
        PL_DELETE_COPY_MOVE_CTOR(Log);

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
            return *m_Instance;
        }
    private:
        static Log* m_Instance;
    private:
        std::array<Ref<spdlog::logger>, kMaxLoggerType> m_Logger;
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

#include "utils/macro_util.h"

#define LOG_VAR_STR(Str, Variable)   LOG_INFO(Str, PL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)            LOG_VAR_STR("value of {0} = {1}", Variable)

#endif /* POLOS_UTILS_LOG_H_ */
