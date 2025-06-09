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
using Clock     = std::chrono::high_resolution_clock;
using Duration  = std::chrono::microseconds;
using TimePoint = Clock::time_point;

namespace utils
{

UTILS_EXPORT auto GetTimeNow() -> std::int64_t;
UTILS_EXPORT auto ConvertToMicroseconds(std::chrono::minutes) -> std::int64_t;
UTILS_EXPORT auto ConvertToMicroseconds(std::chrono::seconds) -> std::int64_t;
UTILS_EXPORT auto ConvertToMicroseconds(std::chrono::milliseconds) -> std::int64_t;

}// namespace utils
}// namespace polos

#endif//UTILS_TIME_TIME_H
