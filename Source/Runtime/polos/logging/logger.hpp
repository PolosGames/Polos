//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef LOGGING_LOG_H
#define LOGGING_LOG_H

#include "polos/logging/module_macros.hpp"

#include <array>
#include <quill/Logger.h>

namespace polos::logging
{
namespace detail
{
    enum logger_type : std::uint8_t
    {
        k_LoggerPolos,
        k_LoggerPolly,
        k_LoggerApp,
        k_MaxLoggers
    };
} // namespace detail

class LOGGING_EXPORT Logger
{
public:
    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;

    static Logger& Instance();

    quill::Logger* GetPolosLogger() const;
    quill::Logger* GetPollyLogger() const;
    quill::Logger* GetAppLogger() const;
private:
    Logger();
};

} // namespace polos::logging

#endif //LOGGING_LOG_H
