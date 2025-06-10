//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/utils/time.hpp"

namespace polos::utils
{

auto GetTimeNow() -> TimePoint
{
    return std::chrono::time_point_cast<Duration>(Clock::now());
}

}// namespace polos::utils
