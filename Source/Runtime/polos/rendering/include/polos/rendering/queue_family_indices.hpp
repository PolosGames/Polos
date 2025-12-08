///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_QUEUE_FAMILY_INDICES_HPP
#define POLOS_RENDERING_QUEUE_FAMILY_INDICES_HPP

#include <cstdint>
#include <limits>

namespace polos::rendering
{

struct alignas(16) queue_family_indices// NOLINT
{
    std::uint32_t gfx_q_index{std::numeric_limits<std::uint32_t>::max()};
    std::uint32_t transfer_q_index{std::numeric_limits<std::uint32_t>::max()};
    std::uint32_t compute_q_index{std::numeric_limits<std::uint32_t>::max()};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_QUEUE_FAMILY_INDICES_HPP
