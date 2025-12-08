///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_RENDER_CONTEXT_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/i_render_context.hpp"
#include "polos/rendering/i_render_system.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/queue_family_indices.hpp"
#include "polos/rendering/texture_2d.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::platform
{
class PlatformManager;
}// namespace polos::platform

namespace polos::communication
{
struct window_framebuffer_resize;
}// namespace polos::communication

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;
class VulkanResourceManager;
class PipelineCache;
class RenderGraph;
struct render_pass_layout_description;

class RENDERING_EXPORT RenderContext : public IRenderContext
{
public:
    RenderContext();
    ~RenderContext() override;
    RenderContext(RenderContext&&)            = delete;
    RenderContext(RenderContext&)             = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&)  = delete;

    auto Initialize(GLFWwindow* t_window) -> Result<void> override;
    auto Shutdown() -> Result<void> override;

    auto BeginFrame() -> VkCommandBuffer override;
    auto EndFrame() -> void override;

    [[nodiscard]] auto GetRenderGraph() const -> IRenderGraph& override;
    [[nodiscard]] auto IsInitialized() const -> bool override;

    auto GetVkSurface() -> VkSurfaceKHR;
    auto GetGfxQueue() -> VkQueue;
    auto GetSwapchain() -> VulkanSwapchain&;

    auto GetCurrentFrameTexture() -> Result<std::shared_ptr<texture_2d>>;
    auto CreateRenderPass(render_pass_layout_description const& t_layout) -> Result<VkRenderPass>;
    auto AddFramebufferToCurrentFrame(VkFramebuffer t_fbuf) -> void;
private:
    friend class platform::PlatformManager;

    void onFramebufferResize();

    GLFWwindow*                                            m_window{nullptr};
    std::unique_ptr<VulkanContext>                         m_context;
    std::unique_ptr<VulkanDevice>                          m_device;
    std::unique_ptr<VulkanSwapchain>                       m_swapchain;
    std::unique_ptr<VulkanResourceManager>                 m_vrm;
    std::unique_ptr<PipelineCache>                         m_pipeline_cache;
    std::unique_ptr<RenderGraph>                           m_render_graph;
    std::vector<std::unique_ptr<rendering::IRenderSystem>> m_render_systems;

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

    enum class ImageAcqusitionResult : std::uint8_t
    {
        kSuccess,
        kError,
    };
    std::array<ImageAcqusitionResult, kMaxFramesInFlight> m_image_acq_results;

    VkSurfaceKHR         m_surface{VK_NULL_HANDLE};
    VkQueue              m_gfx_queue{VK_NULL_HANDLE};
    queue_family_indices m_queue_family_indices;

    bool m_framebuffer_resized{false};

    bool m_is_initialized{false};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_CONTEXT_HPP
