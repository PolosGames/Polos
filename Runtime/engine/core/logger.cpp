#include "plpch.h"
#include "logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace polos
{
	std::shared_ptr<spdlog::logger> logger::_client_logger;
	std::shared_ptr<spdlog::logger> logger::_core_logger;

	logger::logger()
	{
		std::vector<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Polos.log", true));

		log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
		log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

		_core_logger = std::make_shared<spdlog::logger>("POLOS", begin(log_sinks), end(log_sinks));
		spdlog::register_logger(_core_logger);
		_core_logger->set_level(spdlog::level::trace);
		_core_logger->flush_on(spdlog::level::trace);

		_client_logger = std::make_shared<spdlog::logger>("SENAZ", begin(log_sinks), end(log_sinks));
		spdlog::register_logger(_client_logger);
		_client_logger->set_level(spdlog::level::trace);
		_client_logger->flush_on(spdlog::level::trace);
	}

	std::shared_ptr<spdlog::logger> logger::pick_logger(logger_type type)
	{
		switch (type)
		{
		case logger_type::LOGGER_CORE: return _core_logger;
		case logger_type::LOGGER_CLIENT: return _client_logger;
		default: return std::shared_ptr<spdlog::logger>();
		}
	}

}