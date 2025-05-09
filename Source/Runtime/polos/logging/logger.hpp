//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef LOGGING_LOG_H
#define LOGGING_LOG_H

#include "polos/logging/module_macros.hpp"

#include <quill/Logger.h>

namespace polos::logging
{

/// Get the sink name for the console logger
/// @return a string view to the name of the sink
[[nodiscard]] LOGGING_EXPORT std::string_view GetConsoleSinkName();

/// Flush the designated logger immediately while blocking the calling thread.
/// @param t_logger The logger whose sink that needs flushing
LOGGING_EXPORT void FlushLogger(quill::Logger* t_logger);

class LOGGING_EXPORT Logger
{
public:
    Logger(Logger const&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger const&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& Instance();

    /// Get pointer for the quill logger that will be used for logging inside Polos framework
    /// @return Pointer to the logger with id "POLOS"
    [[nodiscard]] quill::Logger* GetPolosLogger() const;

    /// Get pointer for the quill logger that will be used for logging inside Polly Editor
    /// @return Pointer to the logger with id "POLLY"
    [[nodiscard]] quill::Logger* GetPollyLogger() const;

    /// Get pointer for the quill logger that will be used for logging inside Apps made from Polos
    /// @return Pointer to the logger with id "APP"
    [[nodiscard]] quill::Logger* GetAppLogger() const;
private:
    Logger();

    quill::Logger* m_polos_logger;
    quill::Logger* m_polly_logger;
    quill::Logger* m_app_logger;
};

//
// template<typename... Args>
// void verbose(quill::Logger* t_logger, char const* t_format, Args&&... args)
// {
//     QUILL_LOG_TRACE_L1(t_logger, t_format, std::forward<Args>(args)...);
// }
//
// template<typename... Args>
// void info(quill::Logger* t_logger, char const* t_format, Args&&... args)
// {
//     QUILL_LOG_INFO(t_logger, t_format, std::forward<Args>(args)...);
// }
//
// template<typename... Args>
// void warn(quill::Logger* t_logger, char const* t_format, Args&&... args)
// {
//     QUILL_LOG_WARNING(t_logger, t_format, std::forward<Args>(args)...);
// }
//
// template<typename... Args>
// void error(quill::Logger* t_logger, char const* t_format, Args&&... args)
// {
//     QUILL_LOG_ERROR(t_logger, t_format, std::forward<Args>(args)...);
// }
//
// template<typename... Args>
// void critical(quill::Logger* t_logger, char const* t_format, Args&&... args)
// {
//     QUILL_LOG_CRITICAL(t_logger, t_format, std::forward<Args>(args)...);
// }
//
// #ifdef PL_LOGGER_TYPE
//
// template<typename... Args>
// void verbose(char const* t_format, Args&&... args)
// {
//
// }
//
// template<typename... Args>
// void info(char const* t_format, Args&&... args);
//
// template<typename... Args>
// void warn(char const* t_format, Args&&... args);
//
// template<typename... Args>
// void error(char const* t_format, Args&&... args);
//
// template<typename... Args>
// void critical(char const* t_format, Args&&... args);
//
// #endif // PL_LOG_CONTEXT

} // namespace polos::logging

#endif //LOGGING_LOG_H
