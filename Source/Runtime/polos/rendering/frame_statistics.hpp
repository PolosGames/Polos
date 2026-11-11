///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_FRAME_STATISTICS_HPP
#define POLOS_RENDERING_FRAME_STATISTICS_HPP

#include <cstdint>

namespace polos::rendering
{
struct FrameStatistics
{
    int32_t drawCalls{0};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_FRAME_STATISTICS_HPP
