#pragma once
#ifndef POLOS_UTILS_LOGGER_H
#define POLOS_UTILS_LOGGER_H

#include <spdlog/spdlog.h>

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
		static Log& instance()
		{
			static Log l;
			return l;
		}

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

	private:
		std::shared_ptr<spdlog::logger> pick_logger(logger_type type);

		Log();
		Log(Log const&);
	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;
	};

	template<typename ...Args>
	inline void Log::critical(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->critical(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::error(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->error(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::warn(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->warn(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::info(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->info(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void Log::trace(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->trace(fmt, std::forward<Args>(args)...);
	}

} // namespace polos

#define LOG_CORE_CRITICAL(...) ::polos::Log::instance().critical(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_ERROR(...) ::polos::Log::instance().error(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_WARN(...)  ::polos::Log::instance().warn(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_INFO(...)  ::polos::Log::instance().info(::polos::logger_type::kLoggerCore, __VA_ARGS__)
#define LOG_CORE_TRACE(...)  ::polos::Log::instance().trace(::polos::logger_type::kLoggerCore, __VA_ARGS__)

#define LOG_CRITICAL(...) ::polos::Log::instance().critical(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_ERROR(...) ::polos::Log::instance().error(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_WARN(...)  ::polos::Log::instance().warn(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_INFO(...)  ::polos::Log::instance().info(::polos::logger_type::kLoggerClient, __VA_ARGS__)
#define LOG_TRACE(...)  ::polos::Log::instance().trace(::polos::logger_type::kLoggerClient, __VA_ARGS__)

#endif /* POLOS_UTILS_LOGGER_H */
