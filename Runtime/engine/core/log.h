#pragma once
#ifndef POLOS_UTILS_LOG_H_
#define POLOS_UTILS_LOG_H_

#include <spdlog/spdlog.h>

#include "utils/macro_util.h"

namespace polos
{
	enum class logger_type
	{
		kLoggerCore,
		kLoggerClient,
	};

	class Log
	{
	public:
		Log()  = default;
		~Log() = default;
		PL_DELETE_COPY_MOVE_CTOR(Log);

		void Startup();
		void Shutdown();

		template<typename ...Args>
		void critical(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args);

		template<typename ...Args>
		void error(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args);

		template<typename ...Args>
		void warn(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args);

		template<typename ...Args>
		void info(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args);

		template<typename ...Args>
		void trace(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args);

		static Log& Instance()
		{
			return *m_Instance;
		}
	private:
		static Log* m_Instance;

		spdlog::logger& pick_logger(logger_type type);
	private:
		std::shared_ptr<spdlog::logger> m_CoreLogger;
		std::shared_ptr<spdlog::logger> m_ClientLogger;
	};

	template<typename ...Args>
	inline void Log::critical(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type).critical(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::error(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type).error(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::warn(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type).warn(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::info(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type).info(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::trace(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type).trace(fmt, std::forward<Args>(args)...);
	}

} // namespace polos

#define LOG_CORE_CRITICAL(...) ::polos::Log::Instance().critical(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_ERROR(...) ::polos::Log::Instance().error(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_WARN(...)  ::polos::Log::Instance().warn(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_INFO(...)  ::polos::Log::Instance().info(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_TRACE(...)  ::polos::Log::Instance().trace(::polos::logger_type::kLoggerCore, __VA_ARGS__)

#define LOG_CRITICAL(...) ::polos::Log::Instance().critical(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_ERROR(...) ::polos::Log::Instance().error(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_WARN(...)  ::polos::Log::Instance().warn(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_INFO(...)  ::polos::Log::Instance().info(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_TRACE(...)  ::polos::Log::Instance().trace(::polos::logger_type::kLoggerClient, __VA_ARGS__)

#include "utils/macro_util.h"

#define LOG_VAR_STR(Str, Variable)		LOG_INFO(Str, PL_STRINGIFY(Variable), Variable)
#define LOG_VAR(Variable)				LOG_VAR_STR("value of {0} = {1}", Variable)

#endif /* POLOS_UTILS_LOG_H_ */
