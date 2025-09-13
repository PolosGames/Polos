//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_CONTEXT_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_CONTEXT_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/queue_family_indices.hpp"
#include "polos/rendering/vulkan_context.hpp"
#include "polos/rendering/vulkan_device.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"


#include <memory>
#include <vulkan/vulkan_core.h>

namespace polos::rendering
{

class RENDERING_EXPORT RenderContext
{
public:
    explicit RenderContext(GLFWwindow* t_window);
    ~RenderContext();
    RenderContext(RenderContext&&)            = delete;
    RenderContext(RenderContext&)             = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&)  = delete;

    static auto Instance() -> RenderContext const&;

    auto Initialize() -> Result<void>;
    auto Shutdown() -> Result<void>;

    auto GetVkSurface() -> VkSurfaceKHR;
    auto GetGfxQueue() -> VkQueue;
private:
    static RenderContext* s_instance;
    static bool           s_is_initialized;

    GLFWwindow* m_window{nullptr};

    std::unique_ptr<VRM>             m_vrm;
    std::unique_ptr<VulkanContext>   m_context;
    std::unique_ptr<VulkanDevice>    m_device;
    std::unique_ptr<VulkanSwapchain> m_swapchain;

    VkSurfaceKHR         m_surface{VK_NULL_HANDLE};
    VkQueue              m_gfx_queue{VK_NULL_HANDLE};
    queue_family_indices m_queue_family_indices;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_CONTEXT_HPP_
