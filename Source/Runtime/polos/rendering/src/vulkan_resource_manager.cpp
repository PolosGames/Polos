//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_resource_manager.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/allocated_image.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

#include <vulkan/vulkan.h>

namespace polos::rendering
{
std::int32_t VulkanResourceManager::s_resource_id{0};

VulkanResourceManager::VulkanResourceManager()  = default;
VulkanResourceManager::~VulkanResourceManager() = default;

auto VulkanResourceManager::Create(resource_manager_create_details const& t_details) -> Result<void>
{
    m_device    = t_details.device;
    m_allocator = t_details.allocator;
    m_swapchain = t_details.swapchain;

    return {};
}

auto VulkanResourceManager::CreateImage(
    VkImageCreateInfo const& t_image_info,
    VmaAllocationCreateFlags t_flags,
    VmaMemoryUsage           t_usage,
    VkMemoryPropertyFlags    t_memory_property_flags) -> Result<allocated_image*>
{
    m_images.push_back(std::make_unique<allocated_image>());

    auto& image    = m_images.back();
    image->id      = s_resource_id++;
    image->format  = t_image_info.format;
    image->extent  = t_image_info.extent;
    image->samples = t_image_info.samples;

    VmaAllocationCreateInfo alloc_info{
        .flags          = t_flags,
        .usage          = t_usage,
        .requiredFlags  = t_memory_property_flags,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0f,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateImage(m_allocator, &t_image_info, &alloc_info, &image->image, &image->allocation, nullptr),
        RenderingErrc::kFailedCreateImage);

    return image.get();
}

auto VulkanResourceManager::DestroyImage(std::int32_t t_resource_id) -> void
{
    auto itr = std::ranges::find_if(m_images, [t_resource_id](auto const& image) {
        return image->id == t_resource_id;
    });
    if (itr != m_images.end())
    {
        vmaDestroyImage(m_allocator, (*itr)->image, (*itr)->allocation);
        (*itr).reset();
        m_images.erase(itr);
    }
}

auto VulkanResourceManager::CreateBuffer(
    VkBufferCreateInfo const& t_buffer_info,
    VmaAllocationCreateFlags  t_flags,
    VmaMemoryUsage            t_usage,
    VkMemoryPropertyFlags     t_memory_property_flags) -> Result<allocated_buffer*>
{
    m_buffers.push_back(std::make_unique<allocated_buffer>());

    auto& buffer   = m_buffers.back();
    buffer->id     = s_resource_id++;
    buffer->buffer = VK_NULL_HANDLE;
    buffer->usage  = t_buffer_info.usage;

    VmaAllocationCreateInfo alloc_info{
        .flags          = t_flags,
        .usage          = t_usage,
        .requiredFlags  = t_memory_property_flags,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0f,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateBuffer(m_allocator, &t_buffer_info, &alloc_info, &buffer->buffer, &buffer->allocation, nullptr),
        RenderingErrc::kFailedCreateBuffer);

    return buffer.get();
}

auto VulkanResourceManager::DestroyBuffer(std::int32_t t_resource_id) -> void
{
    auto itr = std::ranges::find_if(m_buffers, [t_resource_id](auto const& buffer) {
        return buffer->id == t_resource_id;
    });

    if (itr != m_buffers.end())
    {
        vmaDestroyBuffer(m_allocator, (*itr)->buffer, (*itr)->allocation);
        (*itr).reset();
        m_buffers.erase(itr);
    }
}

auto VulkanResourceManager::Destroy() -> Result<void>// NOLINT
{
    LogInfo("Destroying and invalidating Vulkan Resource Manager...");

    for (auto& texture : m_images) { DestroyImage(texture->id); }
    for (auto& buffer : m_buffers) { DestroyBuffer(buffer->id); }

    m_images.clear();
    m_buffers.clear();
    return {};
}

}// namespace polos::rendering
