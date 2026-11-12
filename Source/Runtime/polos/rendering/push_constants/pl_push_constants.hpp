///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PUSH_CONSTANTS_PL_PUSH_CONSTANTS_HPP
#define POLOS_RENDERING_PUSH_CONSTANTS_PL_PUSH_CONSTANTS_HPP

#include "polos/rendering/push_constants/compute_cull_data.hpp"

#include <cstdint>

namespace polos::rendering
{
template<typename T>
struct PLPushConstants
{
    T data;
};

using ComputeCullPc = PLPushConstants<ComputeCullData>;

}// namespace polos::rendering

#endif// POLOS_RENDERING_PUSH_CONSTANTS_PL_PUSH_CONSTANTS_HPP
