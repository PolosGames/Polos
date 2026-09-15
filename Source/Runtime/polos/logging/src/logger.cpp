///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/logging/logger.hpp"

#include "quill/LogMacros.h"
#include "quill_shared.hpp"

namespace polos::logging
{

std::string_view GetConsoleSinkName()
{
    return "pl_std_sink";
}

Logger::Logger()
{
    setup_quill();

    mPolosLogger = get_logger("POLOS");
    mPollyLogger = get_logger("POLLY");
    mAppLogger   = get_logger("APP");
}

Logger& Logger::Instance()
{
    static Logger sInstance;
    return sInstance;
}

quill::Logger* Logger::GetPolosLogger() const
{ return mPolosLogger; }

quill::Logger* Logger::GetPollyLogger() const
{ return mPollyLogger; }

quill::Logger* Logger::GetAppLogger() const
{ return mAppLogger; }

void FlushLogger(quill::Logger* tLogger)
{
    if (nullptr == tLogger)
    {
        QUILL_LOG_ERROR(Logger::Instance().GetPolosLogger(), "Cannot flush a null logger!");
        return;
    }
    tLogger->flush_log();
}

}// namespace polos::logging
