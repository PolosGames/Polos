#include "polos/polos_pch.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "polos/containers/containers.h"

#include "log.h"

namespace polos
{
    Log* Log::s_Instance;

    void Log::Startup()
    {
        DArray<spdlog::sink_ptr> log_sinks;
        log_sinks.reserve(2);
        log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Polos.log", true));

        log_sinks[0]->set_pattern("%^[%T %n] : %v%$");
        log_sinks[1]->set_pattern("[%T] [%n %l] : %v");
    
        m_Logger[kLoggerEngine] = std::make_shared<spdlog::logger>("ENGINE", std::begin(log_sinks), std::end(log_sinks));
        spdlog::register_logger(m_Logger[logger_type::kLoggerEngine]);
        m_Logger[kLoggerEngine]->set_level(spdlog::level::trace);
        m_Logger[kLoggerEngine]->flush_on(spdlog::level::trace);
        
        m_Logger[kLoggerEditor] = std::make_shared<spdlog::logger>("EDITOR", std::begin(log_sinks), std::end(log_sinks));
        spdlog::register_logger(m_Logger[kLoggerEditor]);
        m_Logger[kLoggerEditor]->set_level(spdlog::level::trace);
        m_Logger[kLoggerEditor]->flush_on(spdlog::level::trace);
    
        m_Logger[kLoggerClient] = std::make_shared<spdlog::logger>("CLIENT", std::begin(log_sinks), std::end(log_sinks));
        spdlog::register_logger(m_Logger[kLoggerClient]);
        m_Logger[kLoggerClient]->set_level(spdlog::level::trace);
        m_Logger[kLoggerClient]->flush_on(spdlog::level::trace);

        s_Instance = this;
    }

    void Log::Shutdown()
    {
        spdlog::drop("ENGINE");
        spdlog::drop("EDITOR");
        spdlog::drop("CLIENT");
        s_Instance = nullptr;
    }
}