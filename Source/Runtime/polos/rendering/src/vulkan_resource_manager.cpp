//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_resource_manager.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/allocated_image.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/render_context.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_util.hpp"

#include <stb_image.h>

#include <vulkan/vulkan.h>

namespace polos::rendering
{
std::int32_t VulkanResourceManager::s_resource_id{0};

VulkanResourceManager::VulkanResourceManager() = default;
VulkanResourceManager::~VulkanResourceManager()
{
    for (std::int32_t i{0}; i < s_resource_id; ++i)
    {
        // Which one is it? Image or buffer? We don't know, so we try both. Good enough for now.
        // The destroy functions will check if the resource exists before trying to destroy it.
        DestroyImage(i);
        DestroyBuffer(i);
    }
}

auto VulkanResourceManager::Create(resource_manager_create_details const& t_details) -> Result<void>
{
    m_device    = t_details.device;
    m_allocator = t_details.allocator;
    m_swapchain = t_details.swapchain;

    m_images.reserve(8U);
    m_buffers.reserve(8U);

    return {};
}

auto VulkanResourceManager::CreateImage(
    VkImageCreateInfo const& t_image_info,
    VmaAllocationCreateFlags t_flags,
    VmaMemoryUsage           t_usage,
    VkMemoryPropertyFlags    t_memory_property_flags) -> Result<std::int32_t>
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

    return image->id;
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
    VkMemoryPropertyFlags     t_memory_property_flags) -> Result<std::int32_t>
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

    return buffer->id;
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

auto VulkanResourceManager::LoadTextureToImage(
    char const*          t_texture_path,
    VkFormat             t_format,
    VkImageUsageFlags    t_usage,
    VkImageLayout        t_initial_layout,
    VkImageLayout        t_final_layout,
    VkPipelineStageFlags t_final_pipeline_stage) -> Result<std::int32_t>
{
    int32_t width{0};
    int32_t height{0};

    uint8_t* pixels = stbi_load(t_texture_path, &width, &height, nullptr, STBI_rgb_alpha);
    assert(nullptr != pixels && "Failed to load texture image!");

    VkDeviceSize const image_size = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * 4U;

    VkBufferCreateInfo const img_buf_info{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .size                  = image_size,
        .usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
    };

    std::int32_t buffer_for_texture_index{0};
    {
        auto img_buf = CreateBuffer(
            img_buf_info,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_HOST);

        buffer_for_texture_index = *img_buf;
    }

    vmaCopyMemoryToAllocation(m_allocator, pixels, GetBuffer(buffer_for_texture_index)->allocation, 0, image_size);

    stbi_image_free(pixels);

    VkImageCreateInfo texture_image_info{
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = nullptr,
        .flags     = 0U,
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = t_format,
        .extent =
            {
                .width  = VK_SIZE_CAST(width),
                .height = VK_SIZE_CAST(height),
                .depth  = 1U,
            },
        .mipLevels             = 1U,
        .arrayLayers           = 1U,
        .samples               = VK_SAMPLE_COUNT_1_BIT,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = t_usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = t_initial_layout,
    };

    std::int32_t image_for_texture_index{0};
    {
        auto image_create_res   = CreateImage(texture_image_info, 0U, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
        image_for_texture_index = *image_create_res;
    }

    // Transition image layout to be optimal for receiving data transfer from staging buffer
    {
        VkCommandBuffer command_buffer = RenderContext::BeginSingleTimeCommands();

        util::TransitionImageLayout(
            command_buffer,
            GetImage(image_for_texture_index)->image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT);

        RenderContext::EndSingleTimeCommands(command_buffer);
    }

    // Copy data from staging buffer to texture image
    {
        VkCommandBuffer command_buffer = RenderContext::BeginSingleTimeCommands();

        util::CopyBufferToImage(
            command_buffer,
            GetBuffer(buffer_for_texture_index)->buffer,
            GetImage(image_for_texture_index)->image,
            VkExtent3D{
                .width  = VK_SIZE_CAST(width),
                .height = VK_SIZE_CAST(height),
                .depth  = 1U,
            });

        RenderContext::EndSingleTimeCommands(command_buffer);
    }

    // Transition image layout to be optimal for shader read access
    {
        VkCommandBuffer command_buffer = RenderContext::BeginSingleTimeCommands();

        util::TransitionImageLayout(
            command_buffer,
            GetImage(image_for_texture_index)->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            t_final_layout,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            t_final_pipeline_stage);

        RenderContext::EndSingleTimeCommands(command_buffer);
    }

    DestroyBuffer(buffer_for_texture_index);

    return image_for_texture_index;
}

auto VulkanResourceManager::GetBuffer(std::int32_t t_resource_id) -> allocated_buffer*
{
    for (std::size_t i{0U}; i < m_buffers.size(); ++i)
    {
        if (m_buffers[i]->id == t_resource_id)
        {
            return m_buffers[i].get();
        }
    }

    return nullptr;
}

auto VulkanResourceManager::GetImage(std::int32_t t_resource_id) -> allocated_image*
{
    for (std::size_t i{0U}; i < m_images.size(); ++i)
    {
        if (m_images[i]->id == t_resource_id)
        {
            return m_images[i].get();
        }
    }

    return nullptr;
}

auto VulkanResourceManager::Destroy() -> Result<void>// NOLINT
{
    LogInfo("Destroying and invalidating Vulkan Resource Manager...");

    for (auto& texture : m_images) { vmaDestroyImage(m_allocator, texture->image, texture->allocation); }
    for (auto& buffer : m_buffers) { vmaDestroyBuffer(m_allocator, buffer->buffer, buffer->allocation); }

    m_images.clear();
    m_buffers.clear();
    return {};
}

}// namespace polos::rendering
