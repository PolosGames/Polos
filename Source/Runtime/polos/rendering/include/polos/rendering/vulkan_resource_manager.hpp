///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP
#define POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/allocated_buffer.hpp"
#include "polos/rendering/allocated_image.hpp"

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;
struct allocated_image;

struct alignas(64) resource_manager_create_details// NOLINT
{
    VkDevice         device;
    VmaAllocator     allocator;
    VulkanSwapchain* swapchain;
};

class VulkanResourceManager
{
public:
    VulkanResourceManager();
    ~VulkanResourceManager();

    VulkanResourceManager(VulkanResourceManager const&)            = delete;
    VulkanResourceManager(VulkanResourceManager&&)                 = delete;
    VulkanResourceManager& operator=(VulkanResourceManager const&) = delete;
    VulkanResourceManager& operator=(VulkanResourceManager&&)      = delete;

    auto Create(resource_manager_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto CreateBuffer(
        VkBufferCreateInfo const& t_buffer_info,
        VmaAllocationCreateFlags  t_flags,
        VmaMemoryUsage            t_usage,
        VkMemoryPropertyFlags     t_memory_property_flags = 0U) -> Result<allocated_buffer*>;
    auto DestroyBuffer(std::int32_t t_resource_id) -> void;

    auto CreateImage(
        VkImageCreateInfo const& t_image_info,
        VmaAllocationCreateFlags t_flags,
        VmaMemoryUsage           t_usage,
        VkMemoryPropertyFlags    t_memory_property_flags = 0U) -> Result<allocated_image*>;
    auto DestroyImage(std::int32_t t_resource_id) -> void;

    template<typename T>
    auto GetResource(std::int32_t t_resource_id) -> T
    {
        if constexpr (std::is_same_v<T, allocated_image>)
        {
            return std::find_if(
                m_images.begin(),
                m_images.end(),
                [t_resource_id](std::unique_ptr<allocated_image> const& img) {
                    return img->id == t_resource_id;
                });
        }
        else if constexpr (std::is_same_v<T, allocated_buffer>)
        {
            return m_buffers;
        }
    }
private:
    static std::int32_t s_resource_id;

    VkDevice         m_device{VK_NULL_HANDLE};
    VmaAllocator     m_allocator{VK_NULL_HANDLE};
    VulkanSwapchain* m_swapchain{nullptr};

    std::vector<std::unique_ptr<allocated_image>>  m_images;
    std::vector<std::unique_ptr<allocated_buffer>> m_buffers;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP
