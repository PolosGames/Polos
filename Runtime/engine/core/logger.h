#pragma once
#ifndef POLOS_UTILS_LOGGER_H
#define POLOS_UTILS_LOGGER_H

#include <spdlog/spdlog.h>

namespace polos
{
	enum class logger_type
	{
		LOGGER_CORE,
		LOGGER_CLIENT,
	};

	class logger
	{
	public:
		static logger& instance()
		{
			static logger l;
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

		logger();
		logger(logger const&);
	private:
		static std::shared_ptr<spdlog::logger> _core_logger;
		static std::shared_ptr<spdlog::logger> _client_logger;
	};

	template<typename ...Args>
	inline void logger::critical(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->critical(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void logger::error(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->error(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void logger::warn(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->warn(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void logger::info(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->info(fmt, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	inline void logger::trace(logger_type type, spdlog::format_string_t<Args...> fmt, Args && ...args)
	{
		pick_logger(type)->trace(fmt, std::forward<Args>(args)...);
	}

} // namespace polos

#define LOG_CORE_CRITICAL(...) ::polos::logger::instance().critical(::polos::logger_type::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_ERROR(...) ::polos::logger::instance().error(::polos::logger_type::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_WARN(...)  ::polos::logger::instance().warn(::polos::logger_type::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_INFO(...)  ::polos::logger::instance().info(::polos::logger_type::LOGGER_CORE, __VA_ARGS__)
#define LOG_CORE_TRACE(...)  ::polos::logger::instance().trace(::polos::logger_type::LOGGER_CORE, __VA_ARGS__)

#define LOG_CRITICAL(...) ::polos::logger::instance().critical(::polos::logger_type::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_ERROR(...) ::polos::logger::instance().error(::polos::logger_type::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_WARN(...)  ::polos::logger::instance().warn(::polos::logger_type::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_INFO(...)  ::polos::logger::instance().info(::polos::logger_type::LOGGER_CLIENT, __VA_ARGS__)
#define LOG_TRACE(...)  ::polos::logger::instance().trace(::polos::logger_type::LOGGER_CLIENT, __VA_ARGS__)

#endif /* POLOS_UTILS_LOGGER_H */
