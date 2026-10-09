///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_TIME_HPP
#define POLOS_UTILS_TIME_HPP

#include <chrono>
#include <cstdint>

namespace polos
{

using Clock     = std::chrono::high_resolution_clock;
using Duration  = std::chrono::microseconds;
using TimePoint = std::chrono::time_point<Clock, Duration>;

namespace utils
{

inline auto GetTimeNow() -> TimePoint
{
    return std::chrono::time_point_cast<Duration>(Clock::now());
}

constexpr auto ConvertToSeconds(std::chrono::microseconds const tUsecs) -> float
{
    return static_cast<float>(tUsecs.count()) * 0.001F * 0.001F;// NOLINT
}

constexpr auto ConvertToMicroseconds(std::chrono::milliseconds const tMsecs) -> Duration
{ return std::chrono::duration_cast<Duration>(tMsecs); }

constexpr auto ConvertToMicroseconds(std::chrono::seconds const tSecs) -> Duration
{
    return ConvertToMicroseconds(std::chrono::milliseconds{tSecs.count() * 1000});// NOLINT
}

constexpr auto ConvertToMicroseconds(std::chrono::minutes const tMins) -> Duration
{
    return ConvertToMicroseconds(std::chrono::seconds{tMins * 60});// NOLINT
}

}// namespace utils
}// namespace polos

constexpr polos::Duration operator""_min(unsigned long long tTime)
{ return polos::utils::ConvertToMicroseconds(std::chrono::minutes(tTime)); }
constexpr polos::Duration operator""_sec(unsigned long long tTime)
{ return polos::utils::ConvertToMicroseconds(std::chrono::seconds(tTime)); }
constexpr polos::Duration operator""_ms(unsigned long long tTime)
{ return polos::utils::ConvertToMicroseconds(std::chrono::milliseconds(tTime)); }

#endif// POLOS_UTILS_TIME_HPP
