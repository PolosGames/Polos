///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/vulkan_pipeline.hpp"

#include "polos/rendering/vk/vulkan_device.hpp"

namespace polos::rendering
{

void VulkanPipeline::Reset(VulkanPipeline& tPipeline)
{
    if (tPipeline.pipeline == VK_NULL_HANDLE && tPipeline.layout == VK_NULL_HANDLE)
    {
        return;
    }

    VkDevice device = VulkanDevice::Get().logiDevice;
    vkDestroyPipeline(device, tPipeline.pipeline, nullptr);
    vkDestroyPipelineLayout(device, tPipeline.layout, nullptr);

    tPipeline = {};
}

}// namespace polos::rendering
