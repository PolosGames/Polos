///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_PIPELINE_HPP
#define POLOS_RENDERING_VULKAN_PIPELINE_HPP

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct alignas(16) vulkan_pipeline// NOLINT
{
    VkPipeline       pipeline{VK_NULL_HANDLE};
    VkPipelineLayout layout{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_PIPELINE_HPP
