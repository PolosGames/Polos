///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_SCOPED_TIMER_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_SCOPED_TIMER_HPP_

#include "polos/utils/macro_utilities.hpp"
#include "polos/utils/time.hpp"

namespace polos::utils
{

/// Used for timing scopes.
///
/// E.g:
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
    explicit ScopedTimer(char const* t_name);
    ~ScopedTimer();

    ScopedTimer(ScopedTimer const&)            = delete;
    ScopedTimer(ScopedTimer&&)                 = delete;
    ScopedTimer& operator=(ScopedTimer&&)      = delete;
    ScopedTimer& operator=(ScopedTimer const&) = delete;
private:
    class Impl;
    Impl* m_impl;
};
}// namespace polos::utils

/// Preferred method of creating a ScopedTimer. Needs to be called at the top of the scope preferably.
/// @param NAME quoted string name of the ScopedTimer
#define CREATE_SCOPED_TIMER(NAME) polos::utils::ScopedTimer PL_ANON_NAME(scopedtimer)(NAME)

#endif// POLOS_UTILS_INCLUDE_POLOS_UTILS_SCOPED_TIMER_HPP_
