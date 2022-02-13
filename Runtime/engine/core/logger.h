#pragma once
#ifndef POLOS_UTILS_LOGGER_H
#define POLOS_UTILS_LOGGER_H

#include <spdlog/spdlog.h>

namespace polos
{
	enum class LoggerType
	{
		LOGGER_CORE,
		LOGGER_CLIENT,
	};

	class Logger
	{
	public:
		static Logger &instance()
		{
			static Logger l;
			return l;
		}

		template<typename... Args>
		void critical(LoggerType type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->critical(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void error(LoggerType type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void warn(LoggerType type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void info(LoggerType type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->error(fmt, std::forward<Args>(args)...);
		}

	private:
		std::shared_ptr<spdlog::logger> pick_logger(LoggerType type);

		Logger();
		Logger(Logger const&);
	private:
		static std::shared_ptr<spdlog::logger> _core_logger;
		static std::shared_ptr<spdlog::logger> _client_logger;
	};
}

#define LOG_CORE_CRITICAL(...) ::polos::Logger::instance().critical(::polos::LoggerType::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_ERROR(...) ::polos::Logger::instance().error(::polos::LoggerType::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_WARN(...)  ::polos::Logger::instance().warn(::polos::LoggerType::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_INFO(...)  ::polos::Logger::instance().info(::polos::LoggerType::LOGGER_CORE, __VA_ARGS__)

#define LOG_CRITICAL(...) ::polos::Logger::instance().critical(::polos::LoggerType::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_ERROR(...) ::polos::Logger::instance().error(::polos::LoggerType::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_WARN(...)  ::polos::Logger::instance().warn(::polos::LoggerType::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_INFO(...)  ::polos::Logger::instance().info(::polos::LoggerType::LOGGER_CLIENT, __VA_ARGS__)

#endif /* POLOS_UTILS_LOGGER_H */
