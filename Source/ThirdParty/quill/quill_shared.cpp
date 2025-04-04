//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "quill_shared.hpp"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>
#include <string>

// Define a global variable for a logger to avoid looking up the logger each time.
// Additional global variables can be defined for additional loggers if needed.

namespace polos::logging
{
QUILL_EXPORT quill::Logger* g_polos_logger;
QUILL_EXPORT quill::Logger* g_polly_logger;
QUILL_EXPORT quill::Logger* g_app_logger;

void setup_quill()
{
    quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);

    std::string const fmt_pattern =
        "[%(time)] %(log_level:<3) %(logger:<6) [%(short_source_location)] %(message)";

    std::string const time_pattern = "%H:%M:%S.%Qms";

    auto std_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("pl_std_sink");

    g_polos_logger = quill::Frontend::create_or_get_logger("POLOS", std_sink, fmt_pattern, time_pattern);
    g_polly_logger = quill::Frontend::create_or_get_logger("POLLY", std_sink, fmt_pattern, time_pattern);
    g_app_logger   = quill::Frontend::create_or_get_logger("APP", std_sink, fmt_pattern, time_pattern);

    g_polos_logger->set_log_level(quill::LogLevel::TraceL1);
    g_polly_logger->set_log_level(quill::LogLevel::TraceL1);
    g_app_logger->set_log_level(quill::LogLevel::TraceL1);
}

quill::Logger* get_logger(std::string const& name)
{
    return quill::Frontend::get_logger(name);
}
} // namespace polos::logging
