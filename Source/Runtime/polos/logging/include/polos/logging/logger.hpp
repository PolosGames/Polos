///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_LOGGING_LOGGER_HPP
#define POLOS_LOGGING_LOGGER_HPP

#include "polos/polos_api.hpp"

#include <quill/Logger.h>

namespace polos::logging
{

/// Get the sink name for the console logger
/// @return a string view to the name of the sink
[[nodiscard]] POLOS_API std::string_view GetConsoleSinkName();

/// Flush the designated logger immediately while blocking the calling thread.
/// @param tLogger The logger whose sink that needs flushing
POLOS_API void FlushLogger(quill::Logger* tLogger);

class POLOS_API Logger
{
public:
    ~Logger() = default;

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

    quill::Logger* mPolosLogger;
    quill::Logger* mPollyLogger;
    quill::Logger* mAppLogger;
};

}// namespace polos::logging

#endif// POLOS_LOGGING_LOGGER_HPP
