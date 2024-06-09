
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
        quill::Backend::start();

        auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("pl_console_sink");

        std::string const format_pattern = 
            "%(time) LOG_%(log_level_id) %(logger:<8) [%(caller_function)] %(message)";

        std::string const time_pattern = "%H:%M:%S.%Qms";

        m_Loggers[k_LoggerEngine] = UniquePtr<quill::Logger>(
            quill::Frontend::create_or_get_logger("engine", console_sink, format_pattern, time_pattern)
        );
        m_Loggers[k_LoggerEditor] = UniquePtr<quill::Logger>(
            quill::Frontend::create_or_get_logger("editor", console_sink, format_pattern, time_pattern)
        );
        m_Loggers[k_LoggerClient] = UniquePtr<quill::Logger>(
            quill::Frontend::create_or_get_logger("client", console_sink, format_pattern, time_pattern)
        );

        for(auto& logger: m_Loggers)
        {
            logger->set_log_level(quill::LogLevel::TraceL1);
        }

        s_Instance = this;
    }

    void Log::Shutdown()
    {
        s_Instance = nullptr;
    }

    quill::Logger* Log::GetEngineLogger()
    {
        return s_Instance->m_Loggers[k_LoggerEngine].get();
    }

    quill::Logger* Log::GetEditorLogger()
    {
        return s_Instance->m_Loggers[k_LoggerEditor].get();
    }

    quill::Logger* Log::GetClientLogger()
    {
        return s_Instance->m_Loggers[k_LoggerClient].get();
    }
}