///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_RESOURCES_IMAGE_DESCRIPTION_HPP
#define POLOS_RENDERING_SRC_RESOURCES_IMAGE_DESCRIPTION_HPP

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::rendering
{

struct ImageDescription
{
    VkExtent3D            extent{.width = 0U, .height = 0U, .depth = 1U};
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkImageUsageFlags     usage{0U};
    std::uint32_t         mipLevels{1U};
    std::uint32_t         arrayLayers{1U};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_IMAGE_DESCRIPTION_HPP
