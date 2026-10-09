///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_COMMON_HPP
#define POLOS_RENDERING_COMMON_HPP

#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#define CHECK_VK_SUCCESS_OR_ERR(Result, Errc) \
    if (VK_SUCCESS != (Result))               \
    {                                         \
        return polos::ErrorType{(Errc)};      \
    }// NOLINT

#define VK_SIZE_CAST(Var) static_cast<std::uint32_t>(Var)// NOLINT

namespace polos::rendering::common
{

// Linear RGB for the sRGB brand color #8298A4. Blitting into the sRGB swapchain encodes it for display.
constexpr std::float_t const kPolosRed{0.22322796F};
constexpr std::float_t const kPolosGreen{0.31398871F};
constexpr std::float_t const kPolosBlue{0.37123768F};

constexpr VkClearValue kClearValue{
    .color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0F}},
};

}// namespace polos::rendering::common

#endif// POLOS_RENDERING_COMMON_HPP
