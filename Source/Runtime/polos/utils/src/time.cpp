//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/utils/time.hpp"

namespace polos::utils
{

auto GetTimeNow() -> std::int64_t
{
    auto const now = std::chrono::duration_cast<Duration>(Clock::now().time_since_epoch());
    return static_cast<std::int64_t>(now.count());
}

auto ConvertToMicroseconds(std::chrono::minutes const t_mins) -> std::int64_t
{
    return ConvertToMicroseconds(std::chrono::seconds{t_mins * 60});
}

auto ConvertToMicroseconds(std::chrono::seconds const t_secs) -> std::int64_t
{
    return ConvertToMicroseconds(std::chrono::milliseconds{t_secs.count() * 1000});
}

auto ConvertToMicroseconds(std::chrono::milliseconds const t_msecs) -> std::int64_t
{
    return t_msecs.count() * 1000;
}

}// namespace polos::utils
