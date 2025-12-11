///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_ALLOCATED_IMAGE_HPP
#define POLOS_RENDERING_ALLOCATED_IMAGE_HPP

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::rendering
{

struct alignas(32) allocated_image// NOLINT
{
    std::int32_t          id{0U};
    VmaAllocation         allocation{VK_NULL_HANDLE};
    VkDeviceSize          size{0U};
    VkDeviceSize          offset{0U};
    VkImage               image{VK_NULL_HANDLE};
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkExtent3D            extent{.width = 0U, .height = 0U, .depth = 1U};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_ALLOCATED_IMAGE_HPP
