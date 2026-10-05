///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "resources/gpu_image.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "vk/common.hpp"

#include <utility>

namespace polos::rendering
{

VmaAllocator GpuImage::sAllocator{VK_NULL_HANDLE};

GpuImage::GpuImage(GpuImage&& tOther)
{ swap(tOther); }

GpuImage::~GpuImage()
{
    if (VK_NULL_HANDLE != img)
    {
        vmaDestroyImage(sAllocator, img, allocation);
    }
}

auto GpuImage::operator=(GpuImage&& tOther) -> GpuImage&
{
    if (this == &tOther)
    {
        return *this;
    }

    destroy();
    swap(tOther);

    return *this;
}

auto GpuImage::Create(ImageDescription const& tDesc) -> Result<std::unique_ptr<GpuImage>>
{
    VkImageCreateInfo const imgInfo{
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

    VmaAllocationCreateInfo const allocInfo{
        .flags          = 0U,
        .usage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0F,
    };

    auto* image = new GpuImage();

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateImage(sAllocator, &imgInfo, &allocInfo, &image->img, &image->allocation, nullptr),
        RenderingErrc::kFailedCreateImage);

    return std::unique_ptr<GpuImage>(image);
}

auto GpuImage::destroy() -> void
{
    if (img != VK_NULL_HANDLE)
    {
        LogInfo("Destroying VkImage while moving the resource. Is this intended?");
        vmaDestroyImage(sAllocator, img, allocation);
    }

    allocation = VK_NULL_HANDLE;
    img        = VK_NULL_HANDLE;
}

auto GpuImage::swap(GpuImage& tOther) -> void
{
    std::swap(allocation, tOther.allocation);
    std::swap(img, tOther.img);
    std::swap(extent, tOther.extent);
    std::swap(format, tOther.format);
}

}// namespace polos::rendering
