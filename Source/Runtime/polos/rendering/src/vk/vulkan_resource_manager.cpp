//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "vk/vulkan_resource_manager.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "resources/allocated_image.hpp"
#include "vk/common.hpp"
#include "vk/render_context.hpp"
#include "vk/vulkan_util.hpp"

#include <stb_image.h>

#include <vulkan/vulkan.h>

namespace polos::rendering
{

std::int32_t           VulkanResourceManager::sResourceId{0};
VulkanResourceManager* VulkanResourceManager::sInstance{nullptr};

VulkanResourceManager::VulkanResourceManager()
{ sInstance = this; };
VulkanResourceManager::~VulkanResourceManager()
{
    for (std::int32_t i{0}; i < sResourceId; ++i)
    {
        // Which one is it? Image or buffer? We don't know, so we try both. Good enough for now.
        // The destroy functions will check if the Resource exists before trying to destroy it.
        DestroyImage(i);
        DestroyBuffer(i);
    }
    sInstance = nullptr;
}

auto VulkanResourceManager::Instance() -> VulkanResourceManager*
{ return sInstance; };

auto VulkanResourceManager::Create(ResourceManagerCreateDetails const& tDetails) -> Result<void>
{
    mDevice    = tDetails.device;
    mAllocator = tDetails.allocator;
    mSwapchain = tDetails.swapchain;

    mImages.reserve(8U);
    mBuffers.reserve(8U);

    return {};
}

auto VulkanResourceManager::CreateImage(ImageDescription const& tDesc) -> Result<std::int32_t>
{
    VkImageCreateInfo const image_info{
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .imageType             = 1U < tDesc.extent.depth ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D,
        .format                = tDesc.format,
        .extent                = tDesc.extent,
        .mipLevels             = tDesc.mipLevels,
        .arrayLayers           = tDesc.arrayLayers,
        .samples               = tDesc.samples,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = tDesc.usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    // images are always device-local here - host visibility would need LINEAR tiling, and
    // uploads stage through a buffer instead
    VmaAllocationCreateInfo const alloc_info{
        .flags          = 0U,
        .usage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0F,
    };

    // allocate before registering, so a failure doesn't leave a null-handled entry behind
    AllocatedImage alloc{.id = sResourceId, .extent = tDesc.extent};
    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateImage(mAllocator, &image_info, &alloc_info, &alloc.image, &alloc.allocation, nullptr),
        RenderingErrc::kFailedCreateImage);

    ++sResourceId;
    mImages.push_back(std::make_unique<AllocatedImage>(alloc));

    return alloc.id;
}

auto VulkanResourceManager::DestroyImage(std::int32_t tResourceId) -> void
{
    auto itr = std::ranges::find_if(mImages, [tResourceId](auto const& tImage) {
        return tImage->id == tResourceId;
    });
    if (itr != mImages.end())
    {
        vmaDestroyImage(mAllocator, (*itr)->image, (*itr)->allocation);
        (*itr).reset();
        mImages.erase(itr);
    }
}

auto VulkanResourceManager::CreateBuffer(BufferDescription const& tDesc) -> Result<std::int32_t>
{
    VkBufferCreateInfo const buffer_info{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .size                  = tDesc.size,
        .usage                 = tDesc.usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
    };

    VmaAllocationCreateInfo const alloc_info{
        .flags          = static_cast<VmaAllocationCreateFlags>(tDesc.hostAccess),
        .usage          = static_cast<VmaMemoryUsage>(tDesc.residence),
        .requiredFlags  = 0U,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0F,
    };

    AllocatedBuffer alloc{.id = sResourceId, .size = tDesc.size};
    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateBuffer(mAllocator, &buffer_info, &alloc_info, &alloc.buffer, &alloc.allocation, nullptr),
        RenderingErrc::kFailedCreateBuffer);

    ++sResourceId;
    mBuffers.push_back(std::make_unique<AllocatedBuffer>(alloc));

    return alloc.id;
}

auto VulkanResourceManager::DestroyBuffer(std::int32_t tResourceId) -> void
{
    auto itr = std::ranges::find_if(mBuffers, [tResourceId](auto const& tBuffer) {
        return tBuffer->id == tResourceId;
    });

    if (itr != mBuffers.end())
    {
        vmaDestroyBuffer(mAllocator, (*itr)->buffer, (*itr)->allocation);
        (*itr).reset();
        mBuffers.erase(itr);
    }
}

auto VulkanResourceManager::LoadImageResourceToVkImage(char const*          tResourcePath,
                                                       VkFormat             tFormat,
                                                       VkImageUsageFlags    tUsage,
                                                       VkImageLayout        tFinalLayout,
                                                       VkPipelineStageFlags tFinalPipelineStage) -> Result<std::int32_t>
{
    std::int32_t width{0};
    std::int32_t height{0};

    std::uint8_t* pixels = stbi_load(tResourcePath, &width, &height, nullptr, STBI_rgb_alpha);
    if (nullptr == pixels)
    {
        return ErrorType{RenderingErrc::kFailedLoadImage};
    }

    VkDeviceSize const image_size = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * 4U;

    std::int32_t buffer_for_texture_index{0};
    {
        auto img_buf = CreateBuffer(BufferDescription{
            .size       = image_size,
            .usage      = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .residence  = MemoryResidence::kHost,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });

        buffer_for_texture_index = *img_buf;
    }

    vmaCopyMemoryToAllocation(mAllocator,
                              pixels,
                              GetAllocatedBuffer(buffer_for_texture_index)->allocation,
                              0,
                              image_size);

    stbi_image_free(pixels);

    std::int32_t image_for_texture_index{0};
    {
        auto image_create_res = CreateImage(ImageDescription{
            .extent = {.width = VK_SIZE_CAST(width), .height = VK_SIZE_CAST(height), .depth = 1U},
            .format = tFormat,
            .usage  = tUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        });

        image_for_texture_index = *image_create_res;
    }

    // Transition image layout to be optimal for receiving data transfer from staging buffer
    {
        VkCommandBuffer command_buffer = RenderContext::BeginSingleTimeCommands();

        util::TransitionImageLayout(command_buffer,
                                    GetImage(image_for_texture_index),
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT);

        RenderContext::EndSingleTimeCommands(command_buffer);
    }

    // Copy data from staging buffer to texture image
    {
        VkCommandBuffer command_buffer = RenderContext::BeginSingleTimeCommands();

        util::CopyBufferToImage(command_buffer,
                                GetBuffer(buffer_for_texture_index),
                                GetImage(image_for_texture_index),
                                VkExtent3D{
                                    .width  = VK_SIZE_CAST(width),
                                    .height = VK_SIZE_CAST(height),
                                    .depth  = 1U,
                                });

        RenderContext::EndSingleTimeCommands(command_buffer);
    }

    // Transition image layout to be optimal for Shader read access
    {
        VkCommandBuffer command_buffer = RenderContext::BeginSingleTimeCommands();

        util::TransitionImageLayout(command_buffer,
                                    GetImage(image_for_texture_index),
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    tFinalLayout,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    tFinalPipelineStage);

        RenderContext::EndSingleTimeCommands(command_buffer);
    }

    DestroyBuffer(buffer_for_texture_index);

    return image_for_texture_index;
}

auto VulkanResourceManager::GetBuffer(std::int32_t tResourceId) -> VkBuffer
{ return GetAllocatedBuffer(tResourceId)->buffer; }

auto VulkanResourceManager::GetImage(std::int32_t tResourceId) -> VkImage
{ return GetAllocatedImage(tResourceId)->image; }

auto VulkanResourceManager::GetAllocatedBuffer(std::int32_t tResourceId) -> AllocatedBuffer*
{
    for (std::size_t i{0U}; i < mBuffers.size(); ++i)
    {
        if (mBuffers[i]->id == tResourceId)
        {
            return mBuffers[i].get();
        }
    }

    return nullptr;
}

auto VulkanResourceManager::GetAllocatedImage(std::int32_t tResourceId) -> AllocatedImage*
{
    for (std::size_t i{0U}; i < mImages.size(); ++i)
    {
        if (mImages[i]->id == tResourceId)
        {
            return mImages[i].get();
        }
    }

    return nullptr;
}

auto VulkanResourceManager::Destroy() -> Result<void>// NOLINT
{
    LogInfo("Destroying and invalidating Vulkan Resource Manager...");

    for (auto& texture : mImages) { vmaDestroyImage(mAllocator, texture->image, texture->allocation); }
    for (auto& buffer : mBuffers) { vmaDestroyBuffer(mAllocator, buffer->buffer, buffer->allocation); }

    mImages.clear();
    mBuffers.clear();
    return {};
}

}// namespace polos::rendering
