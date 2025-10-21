//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_CONTEXT_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_CONTEXT_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/queue_family_indices.hpp"
#include "polos/rendering/texture_2d.hpp"

#include <memory>

namespace polos::platform
{
class WindowManager;
}

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;
class VulkanResourceManager;
class PipelineCache;
class RenderGraph;
struct render_pass_layout_description;

class RENDERING_EXPORT RenderContext
{
public:
    explicit RenderContext(GLFWwindow* t_window);
    ~RenderContext();
    RenderContext(RenderContext&&)            = delete;
    RenderContext(RenderContext&)             = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&)  = delete;

    static auto Instance() -> RenderContext&;

    auto Initialize() -> Result<void>;
    auto Shutdown() -> Result<void>;

    auto GetVkSurface() -> VkSurfaceKHR;
    auto GetGfxQueue() -> VkQueue;

    auto BeginFrame() -> VkCommandBuffer;
    auto EndFrame() -> void;

    auto GetRenderGraph() -> RenderGraph&;
    auto GetSwapchain() -> VulkanSwapchain&;

    auto GetCurrentFrameTexture() -> Result<std::shared_ptr<texture_2d>>;
    auto CreateRenderPass(render_pass_layout_description const& t_layout) -> Result<VkRenderPass>;
    auto AddFramebufferToCurrentFrame(VkFramebuffer t_fbuf) -> void;

    auto IsInitialized() -> bool;
private:
    friend class platform::WindowManager;

    static RenderContext* s_instance;
    static bool           s_is_initialized;

    GLFWwindow*                            m_window{nullptr};
    std::unique_ptr<VulkanContext>         m_context;
    std::unique_ptr<VulkanDevice>          m_device;
    std::unique_ptr<VulkanSwapchain>       m_swapchain;
    std::unique_ptr<VulkanResourceManager> m_vrm;
    std::unique_ptr<PipelineCache>         m_pipeline_cache;
    std::unique_ptr<RenderGraph>           m_render_graph;

    VkCommandPool m_command_pool{VK_NULL_HANDLE};

    std::vector<VkFence>         m_frame_fences;
    std::vector<VkSemaphore>     m_acquire_semaphores;
    std::vector<VkSemaphore>     m_submit_semaphores;
    std::vector<VkCommandBuffer> m_frame_command_buffers;
    std::uint32_t                m_current_frame_index{0U};
    std::uint32_t                m_swapchain_image_index{0U};

    static constexpr std::size_t const                         kMaxFramesInFlight{3U};
    std::vector<VkRenderPass>                                  m_vk_render_passes;
    std::array<std::vector<VkFramebuffer>, kMaxFramesInFlight> m_transient_fbufs;

    VkSurfaceKHR         m_surface{VK_NULL_HANDLE};
    VkQueue              m_gfx_queue{VK_NULL_HANDLE};
    queue_family_indices m_queue_family_indices;

    bool m_is_initialized{false};
};

}// namespace polos::rendering

#if defined(__cplusplus)
extern "C"
{
#endif// __cplusplus
    RENDERING_EXPORT polos::rendering::RenderContext* CreateRenderContext(GLFWwindow* t_window);
    RENDERING_EXPORT void                             DestroyRenderContext(polos::rendering::RenderContext* t_context);
#if defined(__cplusplus)
}
#endif// __cplusplus

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_CONTEXT_HPP_
