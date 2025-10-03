///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VK_INSTANCE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VK_INSTANCE_HPP_

#include "polos/rendering/module_macros.hpp"

#include "polos/rendering/vulkan_state.hpp"

// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <expected>

namespace polos::rendering
{

/// @brief Try to initialize Vulkan with predefined options
/// @return VulkanState with initialized instance and device if successfull.
/// @error A Vulkan error in the form of VkResult
[[nodiscard]] RENDERING_EXPORT auto
InitializeVulkan(GLFWwindow* const t_window) -> std::expected<VulkanState*, VkResult>;

/// @brief Terminate Vulkan and cleanup all resources related to the instance
RENDERING_EXPORT void TerminateVulkan();

/// @brief Render a single frame (asssumes command buffers have been recorded)
RENDERING_EXPORT void RenderFrame();

}// namespace polos::rendering

#if defined(__cplusplus)
extern "C"
{
#endif// __cplusplus

    RENDERING_EXPORT polos::rendering::VulkanState* InitializeVulkan(GLFWwindow* const t_window);
    RENDERING_EXPORT void                           TerminateVulkan();
    RENDERING_EXPORT void                           RenderFrame();

#if defined(__cplusplus)
}
#endif// __cplusplus

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VK_INSTANCE_HPP_
