///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_VULKAN_UTIL_HPP
#define POLOS_RENDERING_SRC_VK_VULKAN_UTIL_HPP

#include "polos/communication/error_code.hpp"
#include "resources/gpu_image.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <span>

namespace polos::rendering::util
{

auto CreateImageView(VkDevice tDevice, VkImage tImage, VkFormat tFormat, VkImageAspectFlags tAspectFlags)
    -> VkImageView;

auto CreateFramebuffer(
    VkDevice               tDevice,
    VkRenderPass           tRPass,
    std::span<VkImageView> tAttachments,
    VkExtent2D const&      tExtent) -> VkFramebuffer;

auto LoadImageResourceToGpuImage(
    char const*          tResourcePath,
    VkFormat             tFormat,
    VkImageUsageFlags    tUsage,
    VkImageLayout        tFinalLayout,
    VkPipelineStageFlags tFinalPipelineStage) -> Result<std::unique_ptr<GpuImage>>;

inline auto To3DExtent(VkExtent2D const& tExtent) -> VkExtent3D
{ return VkExtent3D{.width = tExtent.width, .height = tExtent.height, .depth = 1U}; }

}// namespace polos::rendering::util

constexpr bool operator==(VkExtent2D tLhs, VkExtent2D tRhs)
{ return tLhs.width == tRhs.width && tLhs.height == tRhs.height; }

constexpr bool operator!=(VkExtent2D tLhs, VkExtent2D tRhs)
{ return !operator==(tLhs, tRhs); }

constexpr bool operator==(VkExtent3D tLhs, VkExtent3D tRhs)
{ return tLhs.width == tRhs.width && tLhs.height == tRhs.height && tLhs.depth == tRhs.depth; }

constexpr bool operator!=(VkExtent3D tLhs, VkExtent3D tRhs)
{ return !operator==(tLhs, tRhs); }

constexpr VkExtent2D ToExtent2D(VkExtent3D tSrc)
{
    return {
        .width  = tSrc.width,
        .height = tSrc.height,
    };
}

constexpr VkExtent3D ToExtent3D(VkExtent2D tSrc)
{
    return {
        .width  = tSrc.width,
        .height = tSrc.height,
        .depth  = 1U,
    };
}

#endif// POLOS_RENDERING_SRC_VK_VULKAN_UTIL_HPP
