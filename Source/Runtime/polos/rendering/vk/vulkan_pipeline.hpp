///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_VULKAN_PIPELINE_HPP
#define POLOS_RENDERING_VK_VULKAN_PIPELINE_HPP

#include "polos/polos_api.hpp"

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct POLOS_API VulkanPipeline
{
    VkPipeline       pipeline{VK_NULL_HANDLE};
    VkPipelineLayout layout{VK_NULL_HANDLE};

    static void Reset(VulkanPipeline& tPipeline);
};

}// namespace polos::rendering

#endif
