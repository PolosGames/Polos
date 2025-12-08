///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_ATTACHMENT_USAGE_HPP
#define POLOS_RENDERING_ATTACHMENT_USAGE_HPP

#include "polos/rendering/common.hpp"

#include <vulkan/vulkan.h>

#include <cmath>

namespace polos::rendering
{

constexpr std::float_t const kPolosRed{0.50980395F};
constexpr std::float_t const kPolosGreen{0.59607846F};
constexpr std::float_t const kPolosBlue{0.6431373F};
constexpr VkClearValue const kPolosClear{.color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0F}}};

struct alignas(32) attachment_usage// NOLINT
{
    RenderGraphResourceHandle handle{RenderGraphResourceHandle::Invalid()};
    VkAttachmentLoadOp        load_op{VK_ATTACHMENT_LOAD_OP_DONT_CARE};
    VkAttachmentStoreOp       store_op{VK_ATTACHMENT_STORE_OP_DONT_CARE};
    VkClearValue              clear_value{kPolosClear};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_ATTACHMENT_USAGE_HPP
