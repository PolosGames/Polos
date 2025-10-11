//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_PIPELINE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_PIPELINE_HPP_

#include "polos/rendering/common.hpp"

namespace polos::rendering
{

struct vulkan_pipeline
{
    VkPipeline       pipeline{VK_NULL_HANDLE};
    VkPipelineLayout layout{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_PIPELINE_HPP_
