///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_LOGGING_INCLUDE_POLOS_LOGGING_LOGGER_HPP_
#define POLOS_LOGGING_INCLUDE_POLOS_LOGGING_LOGGER_HPP_

#include "polos/logging/module_macros.hpp"

#include <quill/Logger.h>

namespace polos::logging
{

/// Get the sink name for the console logger
/// @return a string view to the name of the sink
[[nodiscard]] std::string_view GetConsoleSinkName();

/// Flush the designated logger immediately while blocking the calling thread.
/// @param t_logger The logger whose sink that needs flushing
void FlushLogger(quill::Logger* t_logger);

class Logger
{
public:
    Logger(Logger const&)            = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(Logger const&) = delete;
    Logger& operator=(Logger&&)      = delete;

    static auto Instance() -> Logger&;

    /// Get pointer for the quill logger that will be used for logging inside Polos framework
    /// @return Pointer to the logger with id "POLOS"
    [[nodiscard]] auto GetPolosLogger() const -> quill::Logger*;

    /// Get pointer for the quill logger that will be used for logging inside Polly Editor
    /// @return Pointer to the logger with id "POLLY"
    [[nodiscard]] auto GetPollyLogger() const -> quill::Logger*;

    /// Get pointer for the quill logger that will be used for logging inside Apps made from Polos
    /// @return Pointer to the logger with id "APP"
    [[nodiscard]] auto GetAppLogger() const -> quill::Logger*;
private:
    Logger();

    quill::Logger* m_polos_logger;
    quill::Logger* m_polly_logger;
    quill::Logger* m_app_logger;
};

}// namespace polos::logging

#endif// POLOS_LOGGING_INCLUDE_POLOS_LOGGING_LOGGER_HPP_
