///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_SRC_VK_RENDER_CONTEXT_HPP

#include "polos/communication/error_code.hpp"
#include "polos/polos_api.hpp"
#include "vk/frame_data.hpp"
#include "vk/i_render_context.hpp"
#include "vk/queue_family_indices.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::platform
{
class PlatformManager;
}// namespace polos::platform

namespace polos::communication
{
struct WindowFramebufferResize;
}// namespace polos::communication

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;
class VulkanResourceManager;
class ShaderCache;
class PipelineCache;
class RenderCompositor;
struct SceneData;

class POLOS_API RenderContext : public IRenderContext
{
public:
    RenderContext();
    ~RenderContext() override;
    RenderContext(RenderContext&&)            = delete;
    RenderContext(RenderContext&)             = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&)  = delete;

    auto Initialize(IWindowSurface& tSurface) -> Result<void> override;
    auto Shutdown() -> Result<void> override;

    auto BeginFrame() -> VkCommandBuffer override;
    auto EndFrame(SceneData const& tView) -> void override;

    [[nodiscard]] auto IsInitialized() const -> bool override;
    [[nodiscard]] auto GetShaderCache() const -> ShaderCache&;
    [[nodiscard]] auto GetPipelineCache() const -> PipelineCache&;

    [[nodiscard]] static auto BeginSingleTimeCommands() -> VkCommandBuffer;
    static auto               EndSingleTimeCommands(VkCommandBuffer tCommandBuffer) -> void;

    auto GetVkSurface() -> VkSurfaceKHR;
    auto GetGfxQueue() -> VkQueue;
    auto GetSwapchain() -> VulkanSwapchain&;
    auto GetVulkanDevice() -> VulkanDevice&;
    auto GetVulkanResourceManager() -> VulkanResourceManager&;
    auto GetCommandPool() -> VkCommandPool;

    [[nodiscard]] auto GetFramesInFlight() const -> std::uint32_t;
private:
    friend class platform::PlatformManager;
    static RenderContext* sRenderContext;

    void renderFrame(SceneData const& tView);
    void onFramebufferResize();

    IWindowSurface*                        mWindowSurface{nullptr};
    std::unique_ptr<VulkanContext>         mContext;
    std::unique_ptr<VulkanDevice>          mDevice;
    std::unique_ptr<VulkanSwapchain>       mSwapchain;
    std::unique_ptr<VulkanResourceManager> mVrm;
    std::unique_ptr<ShaderCache>           mShaderCache;
    std::unique_ptr<PipelineCache>         mPipelineCache;

    VkCommandPool                mCommandPool{VK_NULL_HANDLE};
    std::vector<VkFence>         mFrameFences;
    std::vector<VkSemaphore>     mAcqSemaphores;
    std::vector<VkSemaphore>     mSubmitSemaphores;
    std::vector<VkCommandBuffer> mFrameCommandBuffers;
    std::uint32_t                mCurrentFrameIndex{0U};
    std::uint32_t                mSwapchainImageIndex{0U};

    static constexpr std::size_t const kMaxFramesInFlight{3U};

    std::array<FrameData, kMaxFramesInFlight> mFrameData;

    VkSurfaceKHR       mSurface{VK_NULL_HANDLE};
    VkQueue            mGfxQueue{VK_NULL_HANDLE};
    QueueFamilyIndices mQueueFamilyIndices;

    bool mHasFramebufferResized{false};

    bool mIsInitialized{false};
    bool isRecording{false};

    // RenderPass specific
    std::unique_ptr<RenderCompositor> mRenderCompositor;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_RENDER_CONTEXT_HPP
