///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_UTIL_HPP
#define POLOS_RENDERING_VULKAN_UTIL_HPP

#include <vulkan/vulkan.h>

#include <string_view>

namespace polos::rendering
{
class VulkanResourceManager;
}// namespace polos::rendering

namespace polos::rendering::util
{

auto CreateImageView(VkDevice t_device, VkImage t_image, VkFormat t_format, VkImageAspectFlags t_aspect_flags)
    -> VkImageView;

void TransitionImageLayout(
    VkCommandBuffer      t_command_buffer,
    VkImage              t_image,
    VkImageLayout        t_old_layout,
    VkImageLayout        t_new_layout,
    VkPipelineStageFlags t_src_stage_mask,
    VkPipelineStageFlags t_dst_stage_mask);

void CopyBufferToImage(VkCommandBuffer t_command_buffer, VkBuffer t_buffer, VkImage t_image, VkExtent3D t_extent);

void CopyImageToImage(
    VkCommandBuffer t_command_buffer,
    VkImage         t_src_image,
    VkImage         t_dst_image,
    VkExtent3D      t_src_extent,
    VkExtent3D      t_dst_extent);


}// namespace polos::rendering::util

#endif// POLOS_RENDERING_VULKAN_UTIL_HPP
