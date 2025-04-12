//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef UTILS_TIME_SCOPED_TIMER_H
#define UTILS_TIME_SCOPED_TIMER_H

#include "polos/utils/time/time.hpp"
#include "polos/platform/platform.hpp"

namespace polos::utils
{
class UTILS_EXPORT ScopedTimer
{
public:
    explicit ScopedTimer(char const* t_name);
    ~ScopedTimer();

    ScopedTimer(ScopedTimer const&) = delete;
    ScopedTimer(ScopedTimer&&) = delete;
    ScopedTimer& operator=(ScopedTimer&&) = delete;
    ScopedTimer& operator=(ScopedTimer const&) = delete;
private:
    class Impl;
    Impl* m_impl;
};
} // namespace polos::utils

#define CREATE_SCOPED_TIMER(NAME) polos::utils::ScopedTimer PL_ANON_NAME(scopedtimer)(NAME)

#endif //UTILS_TIME_SCOPED_TIMER_H
