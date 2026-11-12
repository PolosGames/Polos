///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/vulkan_pipeline.hpp"

#include "polos/rendering/vk/vulkan_device.hpp"

#include <utility>

namespace polos::rendering
{
auto VulkanPipeline::Create(
    VkPipeline                       tPipeline,
    VkPipelineLayout                 tLayout,
    std::span<VkDescriptorSetLayout> tSetLayouts) -> VulkanPipeline
{
    return VulkanPipeline{
        .pipeline       = tPipeline,
        .pipelineLayout = tLayout,
        .dsLayouts      = {tSetLayouts.begin(), tSetLayouts.end()},
    };
}

auto VulkanPipeline::Reset(VulkanPipeline& tPipeline) -> void
{
    VkDevice device = VulkanDevice::Get().logiDevice;

    if (tPipeline.pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, std::exchange(tPipeline.pipeline, VK_NULL_HANDLE), nullptr);
    }
    if (tPipeline.pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, std::exchange(tPipeline.pipelineLayout, VK_NULL_HANDLE), nullptr);
    }
    tPipeline.dsLayouts.clear();
    device = VK_NULL_HANDLE;
}

}// namespace polos::rendering
