#pragma once

#include "core/core.h"

#include <memory>
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace polos 
{
	class POLOSAPI logger
	{
	public:
		enum logger_type
		{
			LOGGER_CORE,
			LOGGER_CLIENT
		};
	public:
		static logger &instance()
		{
			return _instance;
		}

		template<typename... Args>
		void critical(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->critical(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void error(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void warn(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void info(logger_type type, spdlog::format_string_t<Args...> fmt, Args &&... args)
		{
			pick_logger(type)->error(fmt, std::forward<Args>(args)...);
		}

	private:
		static logger &create()
		{
			static logger l;

			(void)_instance;
			return l;
		}

		std::shared_ptr<spdlog::logger> pick_logger(logger_type type);

		logger();
		logger(const logger&);
	private:
		static logger &_instance;

		static std::shared_ptr<spdlog::logger> _core_logger;
		static std::shared_ptr<spdlog::logger> _client_logger;
	};
}

#define PL_CORE_FATAL(...) ::polos::logger::instance().critical(::polos::logger::LOGGER_CORE, __VA_ARGS__)
#define PL_CORE_ERROR(...) ::polos::logger::instance().error(::polos::logger::LOGGER_CORE, __VA_ARGS__)
#define PL_CORE_WARN(...)  ::polos::logger::instance().warn(::polos::logger::LOGGER_CORE, __VA_ARGS__)
#define PL_CORE_INFO(...)  ::polos::logger::instance().info(::polos::logger::LOGGER_CORE, __VA_ARGS__)

#define PL_FATAL(...) ::polos::logger::instance().critical(::polos::logger::LOGGER_CLIENT, __VA_ARGS__)
#define PL_ERROR(...) ::polos::logger::instance().error(::polos::logger::LOGGER_CLIENT, __VA_ARGS__)
#define PL_WARN(...)  ::polos::logger::instance().warn(::polos::logger::LOGGER_CLIENT, __VA_ARGS__)
#define PL_INFO(...)  ::polos::logger::instance().info(::polos::logger::LOGGER_CLIENT, __VA_ARGS__)