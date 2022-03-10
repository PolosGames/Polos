#include "plpch.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "utils/alias.h"

#include "log.h"

namespace polos
{
	Log* Log::m_Instance;

	void Log::Startup()
	{
		DArray<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Polos.log", true));

		log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
		log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

		m_CoreLogger = std::make_shared<spdlog::logger>("POLOS", begin(log_sinks), end(log_sinks));
		spdlog::register_logger(m_CoreLogger);
		m_CoreLogger->set_level(spdlog::level::trace);
		m_CoreLogger->flush_on(spdlog::level::trace);

		m_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(log_sinks), end(log_sinks));
		spdlog::register_logger(m_ClientLogger);
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ClientLogger->flush_on(spdlog::level::trace);

		m_Instance = this;
	}

	void Log::Shutdown()
	{
		m_Instance = nullptr;
	}

	spdlog::logger& Log::pick_logger(logger_type type)
	{
		switch (type)
		{
		case logger_type::kLoggerCore: return *m_CoreLogger;
		case logger_type::kLoggerClient: return *m_ClientLogger;
		default: return *m_CoreLogger;
		}
	}

}