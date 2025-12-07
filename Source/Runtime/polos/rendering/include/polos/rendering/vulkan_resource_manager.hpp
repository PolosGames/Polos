///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP
#define POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP

#include "polos/communication/error_code.hpp"

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
struct texture_2d;

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

    static auto Instance() -> VulkanResourceManager&;

    auto Create(resource_manager_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto CreateImage(
        VkImageCreateInfo const& t_image_info,
        VmaMemoryUsage           t_usage,
        VkImage&                 t_image,
        VmaAllocation&           t_allocation) -> Result<void>;

    auto DestroyImage(VkImage t_image, VmaAllocation t_allocation) -> void;

    auto CreateBuffer(
        VkBufferCreateInfo const& t_buffer_info,
        VmaMemoryUsage            t_usage,
        VkBuffer&                 t_buffer,
        VmaAllocation&            t_allocation) -> Result<void>;

    auto DestroyBuffer(VkBuffer t_buffer, VmaAllocation t_allocation) -> void;

    auto GetTexture(std::string const& t_name) -> VkShaderModule;
private:
    auto onFramebufferResize() -> void;

    friend class RenderContext;
    static VulkanResourceManager* s_instance;

    VkDevice         m_device{VK_NULL_HANDLE};
    VmaAllocator     m_allocator{VK_NULL_HANDLE};
    VulkanSwapchain* m_swapchain{nullptr};

    std::vector<std::shared_ptr<texture_2d>> m_textures;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP
