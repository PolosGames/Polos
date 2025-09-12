//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_STATE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_STATE_HPP_

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct VulkanState
{
    VkInstance     instance;
    VkDevice       device;
    VkQueue        gfx_queue;
    VkSurfaceKHR   surface;
    VkSwapchainKHR swap_chain;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_STATE_HPP_
