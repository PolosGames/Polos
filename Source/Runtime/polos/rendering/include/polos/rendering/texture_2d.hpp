///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_TEXTURE_2D_HPP
#define POLOS_RENDERING_TEXTURE_2D_HPP

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct alignas(32) texture_2d// NOLINT
{
    VkImage               image{VK_NULL_HANDLE};
    VkImageView           view{VK_NULL_HANDLE};
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkExtent2D            extent{.width = 0, .height = 0};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_TEXTURE_2D_HPP
