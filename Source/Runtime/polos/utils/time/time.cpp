//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/utils/time/time.hpp"

namespace polos::utils
{
std::uint64_t GetTimeNow()
{
    auto const now = std::chrono::duration_cast<Duration>(Clock::now().time_since_epoch());
    return static_cast<std::uint64_t>(now.count());
}

} // namespace polos::utils
