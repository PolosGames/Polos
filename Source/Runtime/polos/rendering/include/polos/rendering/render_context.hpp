///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_RENDER_CONTEXT_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/i_render_context.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/passes/general_pass.hpp"
#include "polos/rendering/queue_family_indices.hpp"
#include "polos/rendering/texture_description.hpp"
#include "polos/rendering/vertex.hpp"

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
class ShaderCache;
class PipelineCache;
struct render_pass_layout_description;
struct allocated_image;
struct allocated_buffer;

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

    [[nodiscard]] auto IsInitialized() const -> bool override;
    [[nodiscard]] auto GetShaderCache() const -> ShaderCache&;
    [[nodiscard]] auto GetPipelineCache() const -> PipelineCache&;

    [[nodiscard]] static auto BeginSingleTimeCommands() -> VkCommandBuffer;
    static auto               EndSingleTimeCommands(VkCommandBuffer t_command_buffer) -> void;

    auto GetVkSurface() -> VkSurfaceKHR;
    auto GetGfxQueue() -> VkQueue;
    auto GetSwapchain() -> VulkanSwapchain&;
    auto GetVulkanDevice() -> VulkanDevice&;
    auto GetVulkanResourceManager() -> VulkanResourceManager&;
    auto GetCommandPool() -> VkCommandPool;

    [[nodiscard]] auto GetFramesInFlight() const -> std::uint32_t;
private:
    friend class platform::PlatformManager;
    static RenderContext* s_render_context;

    void renderFrame();
    void onFramebufferResize();

    GLFWwindow*                            m_window{nullptr};
    std::unique_ptr<VulkanContext>         m_context;
    std::unique_ptr<VulkanDevice>          m_device;
    std::unique_ptr<VulkanSwapchain>       m_swapchain;
    std::unique_ptr<VulkanResourceManager> m_vrm;
    std::unique_ptr<ShaderCache>           m_shader_cache;
    std::unique_ptr<PipelineCache>         m_pipeline_cache;

    VkCommandPool                m_command_pool{VK_NULL_HANDLE};
    std::vector<VkFence>         m_frame_fences;
    std::vector<VkSemaphore>     m_acquire_semaphores;
    std::vector<VkSemaphore>     m_submit_semaphores;
    std::vector<VkCommandBuffer> m_frame_command_buffers;
    std::uint32_t                m_current_frame_index{0U};
    std::uint32_t                m_swapchain_image_index{0U};

    static constexpr std::size_t const kMaxFramesInFlight{3U};
    std::vector<texture_description>   m_swapchain_images;

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

    // RenderPass specific
    std::unique_ptr<GeneralPass> m_general_pass;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_CONTEXT_HPP
