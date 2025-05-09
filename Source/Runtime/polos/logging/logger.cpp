//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/logging/logger.hpp"

#include "quill_shared.hpp"
#include "quill/LogMacros.h"

namespace polos::logging
{

std::string_view GetConsoleSinkName()
{
    return "pl_std_sink";
}

Logger::Logger()
{
    setup_quill();

    m_polos_logger = get_logger("POLOS");
    m_polly_logger = get_logger("POLLY");
    m_app_logger = get_logger("APP");

    QUILL_LOG_INFO(m_polos_logger, "Logger initialized!");
}

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

quill::Logger* Logger::GetPolosLogger() const
{
    return m_polos_logger;
}

quill::Logger* Logger::GetPollyLogger() const
{
    return m_polly_logger;
}

quill::Logger* Logger::GetAppLogger() const
{
    return m_app_logger;
}

void FlushLogger(quill::Logger* t_logger)
{
    t_logger->flush_log();
}

} // namespace polos::logging
