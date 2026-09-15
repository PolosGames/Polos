///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_VULKAN_UTIL_HPP
#define POLOS_RENDERING_SRC_VK_VULKAN_UTIL_HPP

#include <vulkan/vulkan.h>

#include <span>

namespace polos::rendering
{
class VulkanResourceManager;
}// namespace polos::rendering

namespace polos::rendering::util
{

auto CreateImageView(VkDevice tDevice, VkImage tImage, VkFormat tFormat, VkImageAspectFlags tAspectFlags)
    -> VkImageView;

void TransitionImageLayout(VkCommandBuffer      tCommandBuffer,
                           VkImage              tImage,
                           VkImageLayout        tOldLayout,
                           VkImageLayout        tNewLayout,
                           VkPipelineStageFlags tSrcStageMask,
                           VkPipelineStageFlags tDstStageMask);

void CopyBufferToImage(VkCommandBuffer tCommandBuffer, VkBuffer tBuffer, VkImage tImage, VkExtent3D tExtent);

void CopyImageToImage(VkCommandBuffer tCommandBuffer,
                      VkImage         tSrcImage,
                      VkImage         tDstImage,
                      VkExtent3D      tSrcExtent,
                      VkExtent3D      tDstExtent);

auto CreateFramebuffer(VkDevice               tDevice,
                       VkRenderPass           tRPass,
                       std::span<VkImageView> tAttachments,
                       VkExtent2D const&      tExtent) -> VkFramebuffer;

inline auto To3DExtent(VkExtent2D const& tExtent) -> VkExtent3D
{ return VkExtent3D{.width = tExtent.width, .height = tExtent.height, .depth = 1U}; }

}// namespace polos::rendering::util

inline constexpr bool operator==(VkExtent2D tLhs, VkExtent2D tRhs)
{ return tLhs.width == tRhs.width && tLhs.height == tRhs.height; }

inline constexpr bool operator!=(VkExtent2D tLhs, VkExtent2D tRhs)
{ return !operator==(tLhs, tRhs); }

#endif// POLOS_RENDERING_SRC_VK_VULKAN_UTIL_HPP
