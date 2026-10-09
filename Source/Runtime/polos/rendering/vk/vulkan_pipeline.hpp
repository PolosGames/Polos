///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_PIPELINE_HPP
#define POLOS_RENDERING_VULKAN_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include <span>

namespace polos::rendering
{

struct alignas(16) VulkanPipeline
{
    VkPipeline                             pipeline{VK_NULL_HANDLE};
    VkPipelineLayout                       pipelineLayout{VK_NULL_HANDLE};
    std::span<VkDescriptorSetLayout const> dsLayouts;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_PIPELINE_HPP
