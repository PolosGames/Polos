
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


        std::string const fmtPattern = 
            "[%(time) %(logger:<6) %(log_level_id)] %(message)";

        std::string const timePattern = "%H:%M:%S.%Qms";

        auto stdSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("pl_stdSink");
        
        m_Loggers[k_LoggerEngine] = 
            quill::Frontend::create_or_get_logger("ENGINE", stdSink, fmtPattern, timePattern);
        
        m_Loggers[k_LoggerEditor] = 
            quill::Frontend::create_or_get_logger("EDITOR", stdSink, fmtPattern, timePattern);
        
        m_Loggers[k_LoggerClient] = 
            quill::Frontend::create_or_get_logger("CLIENT", stdSink, fmtPattern, timePattern);

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
        return s_Instance->m_Loggers[k_LoggerEngine];
    }

    quill::Logger* Log::GetEditorLogger()
    {
        return s_Instance->m_Loggers[k_LoggerEditor];
    }

    quill::Logger* Log::GetClientLogger()
    {
        return s_Instance->m_Loggers[k_LoggerClient];
    }
}