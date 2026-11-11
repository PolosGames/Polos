///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PUSH_CONSTANTS_COMPUTE_PUSH_CONSTANTS_HPP
#define POLOS_RENDERING_PUSH_CONSTANTS_COMPUTE_PUSH_CONSTANTS_HPP

#include <cstdint>

namespace polos::rendering
{
struct ComputePushConstants
{
    uint32_t slotCount{0U};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_PUSH_CONSTANTS_COMPUTE_PUSH_CONSTANTS_HPP
