///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_TIME_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_TIME_HPP_

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

inline constexpr auto ConvertToSeconds(std::chrono::microseconds const t_usecs) -> float
{
    return static_cast<float>(t_usecs.count()) * 0.001f * 0.001f;
}

inline constexpr auto ConvertToMicroseconds(std::chrono::milliseconds const t_msecs) -> Duration
{
    return std::chrono::duration_cast<Duration>(t_msecs);
}

inline constexpr auto ConvertToMicroseconds(std::chrono::seconds const t_secs) -> Duration
{
    return ConvertToMicroseconds(std::chrono::milliseconds{t_secs.count() * 1000});
}

inline constexpr auto ConvertToMicroseconds(std::chrono::minutes const t_mins) -> Duration
{
    return ConvertToMicroseconds(std::chrono::seconds{t_mins * 60});
}

}// namespace utils
}// namespace polos

inline constexpr polos::Duration operator""_min(unsigned long long t_time)
{
    return polos::utils::ConvertToMicroseconds(std::chrono::minutes(t_time));
}
inline constexpr polos::Duration operator""_sec(unsigned long long t_time)
{
    return polos::utils::ConvertToMicroseconds(std::chrono::seconds(t_time));
}
inline constexpr polos::Duration operator""_ms(unsigned long long t_time)
{
    return polos::utils::ConvertToMicroseconds(std::chrono::milliseconds(t_time));
}

#endif//POLOS_UTILS_INCLUDE_POLOS_UTILS_TIME_HPP_
