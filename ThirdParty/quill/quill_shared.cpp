//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "quill_shared.hpp"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/core/PatternFormatterOptions.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/FileSink.h>

#include <string>

#if defined(__MINGW32__)
// Keep the exported definitions out of line so consumers import the same state.
QUILL_BEGIN_NAMESPACE
namespace detail
{
auto ThreadContextManager::instance() noexcept -> ThreadContextManager&
{
    static ThreadContextManager manager;
    return manager;
}

auto LoggerManager::instance() noexcept -> LoggerManager&
{
    static LoggerManager manager;
    return manager;
}

auto SinkManager::instance() noexcept -> SinkManager&
{
    static SinkManager manager;
    return manager;
}

auto BackendManager::instance() noexcept -> BackendManager&
{
    static BackendManager manager;
    return manager;
}

auto SignalHandlerContext::instance() noexcept -> SignalHandlerContext&
{
    static SignalHandlerContext context;
    return context;
}
}// namespace detail
QUILL_END_NAMESPACE
#endif

// Define a global variable for a logger to avoid looking up the logger each time.
// Additional global variables can be defined for additional loggers if needed.

namespace polos::logging
{
QUILL_EXPORT quill::Logger const* g_polos_logger;// NOLINT
QUILL_EXPORT quill::Logger const* g_polly_logger;// NOLINT
QUILL_EXPORT quill::Logger const* g_app_logger;  // NOLINT

void setup_quill()
{
#if defined(NDEBUG)
    static constexpr quill::LogLevel log_level{quill::LogLevel::Info};
#else
    static constexpr quill::LogLevel log_level{quill::LogLevel::Debug};
#endif

    quill::BackendOptions const backend_options{};
    quill::Backend::start(backend_options);

    quill::PatternFormatterOptions const formatter_options{
        "%(log_level:<3): %(logger:<6) [%(short_source_location)] %(message)",// LOG
        "%H:%M:%S.%Qms"                                                       // TIME
    };

    std::shared_ptr<quill::Sink> sink;

    sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("pl_std_sink");
    // sink = quill::Frontend::create_or_get_sink<quill::FileSink>("polos.log", []() {
    //     quill::FileSinkConfig config;
    //     config.set_open_mode('a');
    //     config.set_filename_append_option(quill::FilenameAppendOption::StartDateTime);
    //     return config;
    // }());

    auto* const polos_logger = quill::Frontend::create_or_get_logger("POLOS", sink, formatter_options);
    auto* const polly_logger = quill::Frontend::create_or_get_logger("POLLY", sink, formatter_options);
    auto* const app_logger   = quill::Frontend::create_or_get_logger("APP", sink, formatter_options);

    polos_logger->set_log_level(log_level);
    polly_logger->set_log_level(log_level);
    app_logger->set_log_level(log_level);

    g_polos_logger = polos_logger;
    g_polly_logger = polly_logger;
    g_app_logger   = app_logger;
}

quill::Logger* get_logger(std::string const& name)
{
    return quill::Frontend::get_logger(name);
}
}// namespace polos::logging
