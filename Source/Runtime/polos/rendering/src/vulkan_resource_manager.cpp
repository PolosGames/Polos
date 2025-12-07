//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_resource_manager.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/texture_2d.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

#include <vulkan/vulkan.h>

#include <array>

namespace polos::rendering
{

VulkanResourceManager* VulkanResourceManager::s_instance = nullptr;

VulkanResourceManager::VulkanResourceManager()  = default;
VulkanResourceManager::~VulkanResourceManager() = default;

auto VulkanResourceManager::Instance() -> VulkanResourceManager&
{
    return *s_instance;
}

auto VulkanResourceManager::Create(resource_manager_create_details const& t_details) -> Result<void>
{
    m_device    = t_details.device;
    m_allocator = t_details.allocator;
    m_swapchain = t_details.swapchain;

    VkFormat const   sc_img_fmt = m_swapchain->GetSurfaceFormat().format;
    VkExtent2D const sc_img_ext = m_swapchain->GetExtent();

    // Create swapchain images as textures if they have been created.
    // always make sure they are the first n images.
    // Note: Swapchain images are not allocated with VMA (managed by swapchain)
    std::uint32_t const img_count = m_swapchain->GetImageCount();
    for (std::uint32_t i{0U}; i < img_count; ++i)
    {
        m_textures.push_back(
            std::make_shared<texture_2d>(
                m_swapchain->GetImage(i),
                m_swapchain->GetImageView(i),
                VK_NULL_HANDLE,// No VMA allocation for swapchain images
                sc_img_fmt,
                sc_img_ext,
                VK_SAMPLE_COUNT_1_BIT));
    }

    return {};
}

auto VulkanResourceManager::Destroy() -> Result<void>// NOLINT
{
    // Clean up any VMA-allocated textures
    for (auto& texture : m_textures)
    {
        if (texture && texture->allocation != VK_NULL_HANDLE)
        {
            if (texture->view != VK_NULL_HANDLE)
            {
                vkDestroyImageView(m_device, texture->view, nullptr);
            }
            vmaDestroyImage(m_allocator, texture->image, texture->allocation);
        }
    }
    m_textures.clear();

    return {};
}

auto VulkanResourceManager::CreateImage(
    VkImageCreateInfo const& t_image_info,
    VmaMemoryUsage           t_usage,
    VkImage&                 t_image,
    VmaAllocation&           t_allocation) -> Result<void>
{
    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = t_usage;

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateImage(m_allocator, &t_image_info, &alloc_info, &t_image, &t_allocation, nullptr),
        RenderingErrc::kFailedCreateImage);

    return {};
}

auto VulkanResourceManager::DestroyImage(VkImage t_image, VmaAllocation t_allocation) -> void
{
    vmaDestroyImage(m_allocator, t_image, t_allocation);
}

auto VulkanResourceManager::CreateBuffer(
    VkBufferCreateInfo const& t_buffer_info,
    VmaMemoryUsage            t_usage,
    VkBuffer&                 t_buffer,
    VmaAllocation&            t_allocation) -> Result<void>
{
    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = t_usage;

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateBuffer(m_allocator, &t_buffer_info, &alloc_info, &t_buffer, &t_allocation, nullptr),
        RenderingErrc::kFailedCreateBuffer);

    return {};
}

auto VulkanResourceManager::DestroyBuffer(VkBuffer t_buffer, VmaAllocation t_allocation) -> void
{
    vmaDestroyBuffer(m_allocator, t_buffer, t_allocation);
}

auto VulkanResourceManager::onFramebufferResize() -> void
{
    VkExtent2D const sc_img_ext = m_swapchain->GetExtent();

    LogInfo("Updating texture extents to {}x{} due to framebuffer resize.", sc_img_ext.width, sc_img_ext.height);

    std::uint32_t const img_count = m_swapchain->GetImageCount();
    for (std::uint32_t i{0U}; i < img_count; ++i)
    {
        m_textures[i]->image  = m_swapchain->GetImage(i);
        m_textures[i]->view   = m_swapchain->GetImageView(i);
        m_textures[i]->extent = sc_img_ext;
    }
}

}// namespace polos::rendering
