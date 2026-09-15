///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_RESOURCES_ALLOCATED_BUFFER_HPP
#define POLOS_RENDERING_SRC_RESOURCES_ALLOCATED_BUFFER_HPP

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::rendering
{

struct AllocatedBuffer
{
    std::int32_t  id{0};
    VmaAllocation allocation{VK_NULL_HANDLE};
    VkBuffer      buffer{VK_NULL_HANDLE};
    VkDeviceSize  size{0U};
    VkDeviceSize  offset{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_ALLOCATED_BUFFER_HPP
