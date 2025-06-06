//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef UTILS_TIME_TIME_H
#define UTILS_TIME_TIME_H

#include "polos/utils/module_macros.hpp"

#include <chrono>
#include <cstdint>

namespace polos
{
using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::microseconds;
using TimePoint = Clock::time_point;

namespace utils
{
UTILS_EXPORT std::uint64_t GetTimeNow();
} // namespace utils
} // namespace polos

#endif //UTILS_TIME_TIME_H
