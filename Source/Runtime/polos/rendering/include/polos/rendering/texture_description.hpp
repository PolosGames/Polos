///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_TEXTURE_DESCRIPTION_HPP
#define POLOS_RENDERING_TEXTURE_DESCRIPTION_HPP

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct texture_description
{
    VkImage               image{VK_NULL_HANDLE};
    VkImageView           image_view{VK_NULL_HANDLE};
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_TEXTURE_DESCRIPTION_HPP
