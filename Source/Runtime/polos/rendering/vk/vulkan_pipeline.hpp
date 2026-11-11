///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_PIPELINE_HPP
#define POLOS_RENDERING_VULKAN_PIPELINE_HPP

#include "polos/rendering/vk/descriptor_set_layout.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <span>
#include <vector>

namespace polos::rendering
{

struct VulkanPipeline
{
    [[nodiscard]] static auto
    Create(VkPipeline tPipeline, VkPipelineLayout tLayout, std::span<VkDescriptorSetLayout> tSetLayouts)
        -> VulkanPipeline;

    static auto Reset(VulkanPipeline& tPipeline) -> void;

    VkPipeline                         pipeline{VK_NULL_HANDLE};
    VkPipelineLayout                   pipelineLayout{VK_NULL_HANDLE};
    std::vector<VkDescriptorSetLayout> dsLayouts;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_PIPELINE_HPP
