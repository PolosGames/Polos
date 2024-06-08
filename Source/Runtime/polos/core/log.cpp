
#include "log.h"

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/sinks/ConsoleSink.h"

#include "polos/containers/containers.h"
#include "polos/core/engine/engine.h"

namespace polos
{
    Log* Log::s_Instance;

    Log::Log()
    {
        SUBSCRIBE_TO_ENGINE_STARTUP(Startup);
        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);
    }

    void Log::Startup()
    {
        DArray<spdlog::sink_ptr> log_sinks;
        log_sinks.reserve(2);
        log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Polos.log", true));

        log_sinks[0]->set_pattern("%^[%T %n] : %v%$");
        log_sinks[1]->set_pattern("[%T] [%n %l] : %v");

        m_Loggers[k_LoggerEngine] = std::make_shared<spdlog::logger>("ENGINE", std::begin(log_sinks), std::end(log_sinks));
        spdlog::register_logger(m_Loggers[logger_type::k_LoggerEngine]);
        m_Loggers[k_LoggerEngine]->set_level(spdlog::level::trace);
        m_Loggers[k_LoggerEngine]->flush_on(spdlog::level::trace);

        m_Loggers[k_LoggerEditor] = std::make_shared<spdlog::logger>("EDITOR", std::begin(log_sinks), std::end(log_sinks));
        spdlog::register_logger(m_Loggers[k_LoggerEditor]);
        m_Loggers[k_LoggerEditor]->set_level(spdlog::level::trace);
        m_Loggers[k_LoggerEditor]->flush_on(spdlog::level::trace);

        m_Loggers[k_LoggerClient] = std::make_shared<spdlog::logger>("CLIENT", std::begin(log_sinks), std::end(log_sinks));
        spdlog::register_logger(m_Loggers[k_LoggerClient]);
        m_Loggers[k_LoggerClient]->set_level(spdlog::level::trace);
        m_Loggers[k_LoggerClient]->flush_on(spdlog::level::trace);

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