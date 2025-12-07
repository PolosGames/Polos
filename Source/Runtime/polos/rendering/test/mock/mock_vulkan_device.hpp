///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_TEST_MOCK_VULKAN_DEVICE_HPP
#define POLOS_RENDERING_TEST_MOCK_VULKAN_DEVICE_HPP

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

namespace polos::rendering::test
{

class MockVulkanDevice
{
public:
    MockVulkanDevice()
        : logi_device(reinterpret_cast<VkDevice>(0x1)),        // NOLINT
          phys_device(reinterpret_cast<VkPhysicalDevice>(0x2)),// NOLINT
          allocator(reinterpret_cast<VmaAllocator>(0x3))       // NOLINT
    {}

    VkDevice         logi_device{VK_NULL_HANDLE};
    VkPhysicalDevice phys_device{VK_NULL_HANDLE};
    VmaAllocator     allocator{VK_NULL_HANDLE};
};

}// namespace polos::rendering::test

#endif// POLOS_RENDERING_TEST_MOCK_VULKAN_DEVICE_HPP
