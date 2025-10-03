///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_SCOPED_TIMER_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_SCOPED_TIMER_HPP_

#include "polos/logging/log_macros.hpp"
#include "polos/utils/macro_utilities.hpp"
#include "polos/utils/time.hpp"

namespace polos::utils
{

/// @brief Used for timing scopes.
///
/// @example
/// ```cpp
/// {
///   CREATE_SCOPED_TIMER("Main Loop");
///   std::cout << "Hello from scope";
/// }
///
/// Output:
/// [Starting timer for scope: "Main Loop"]
/// [Scope: "Main Loop", took: 1.0002s]
/// ```
class UTILS_EXPORT ScopedTimer
{
public:
    explicit ScopedTimer(std::string t_name)
        : m_name{t_name},
          m_start{GetTimeNow()}
    {
        LogInfo("Starting timer for scope: \"{}\"", m_name);
    }
    ~ScopedTimer()
    {
        auto const passed_time = GetTimeNow() - m_start;

        LogInfo("Scope: \"{}\", took: {}s", m_name, (static_cast<float>(passed_time.count()) * 0.001f * 0.001f));
    }

    ScopedTimer(ScopedTimer const&)            = delete;
    ScopedTimer(ScopedTimer&&)                 = delete;
    ScopedTimer& operator=(ScopedTimer&&)      = delete;
    ScopedTimer& operator=(ScopedTimer const&) = delete;
private:
    std::string m_name;
    TimePoint   m_start;
};
}// namespace polos::utils

/// Preferred method of creating a ScopedTimer. Needs to be called at the top of the scope preferably.
/// @param NAME quoted string name of the ScopedTimer
#define CREATE_SCOPED_TIMER(NAME) polos::utils::ScopedTimer PL_ANON_NAME(scopedtimer)(NAME)

#endif// POLOS_UTILS_INCLUDE_POLOS_UTILS_SCOPED_TIMER_HPP_
