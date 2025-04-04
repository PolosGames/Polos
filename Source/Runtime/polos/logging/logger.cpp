//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/logging/logger.hpp"

#include "quill_shared.hpp"
#include "quill/LogMacros.h"

namespace polos::logging
{
Logger::Logger()
{
    setup_quill();

    QUILL_LOG_INFO(get_logger("POLOS"), "Logger initialized!");
}

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

quill::Logger* Logger::GetPolosLogger() const
{
    return get_logger("POLOS");
}

quill::Logger* Logger::GetPollyLogger() const
{
    return get_logger("POLLY");
}

quill::Logger* Logger::GetAppLogger() const
{
    return get_logger("APP");
}

} // namespace polos::logging
