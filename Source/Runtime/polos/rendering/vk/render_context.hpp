///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_RENDER_CONTEXT_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/render_subsystem.hpp"
#include "polos/rendering/settings.hpp"
#include "polos/rendering/vk/frame_data.hpp"
#include "polos/rendering/vk/i_render_context.hpp"
#include "polos/rendering/vk/queue_family_indices.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>
#include <memory>

#if defined(ENABLE_PROFILING)
namespace tracy
{
class VkCtx;
}
#endif

namespace polos::platform
{
class PlatformManager;
}// namespace polos::platform

namespace polos::ecs
{
}// namespace polos::ecs

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;
class RenderCompositor;
class TextureCache;
class MeshCache;
class MaterialCache;
class ModelCache;

class POLOS_API RenderContext : public IRenderContext
{
public:
    [[nodiscard]] static auto Get() -> RenderContext&;

    RenderContext();
    ~RenderContext() override;
    RenderContext(RenderContext&&)            = delete;
    RenderContext(RenderContext&)             = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&)  = delete;

    auto Initialize(platform::IWindowSurface& tSurface) -> Result<void> override;
    auto Shutdown() -> Result<void> override;

    auto BeginFrame() -> CommandBuffer* override;
    auto EndFrame() -> FrameStatistics override;

    [[nodiscard]] auto IsInitialized() -> bool;

    [[nodiscard]] auto BeginSingleTimeCommands() -> CommandBuffer;
    [[nodiscard]] auto EndSingleTimeCommands(CommandBuffer& tCmdBuf) -> bool;

#if defined(ENABLE_PROFILING)
    auto GetGpuProfileContext() -> tracy::VkCtx*;
#endif
private:
    friend class platform::PlatformManager;
    static RenderContext* sRenderContext;

    auto createSwapchain(platform::FramebufferSize const& tFbSize) -> Result<void>;
    auto destroySwapchain() -> void;
    auto recreateSwapchain(platform::FramebufferSize const& tFbSize) -> void;
    auto createFrameResources() -> Result<void>;
    auto destroyFrameResources() -> void;

    platform::IWindowSurface*         mWindowSurface{nullptr};
    std::unique_ptr<VulkanContext>    mContext;
    std::unique_ptr<VulkanDevice>     mDevice;
    std::unique_ptr<VulkanSwapchain>  mSwapchain;
    std::unique_ptr<TextureCache>     mTextureCache;
    std::unique_ptr<RenderCompositor> mRenderCompositor;
    std::unique_ptr<MeshCache>        mMeshCache;
    std::unique_ptr<MaterialCache>    mMaterialCache;
    std::unique_ptr<ModelCache>       mModelCache;

    std::array<VkFence, Settings::kMaxFramesInFlight>     mFrameFences{};
    std::array<VkSemaphore, Settings::kMaxFramesInFlight> mAcqSemaphores{};
    std::vector<VkSemaphore>                              mSubmitSemaphores;
    std::uint32_t                                         mCurrentFrameIndex{0U};
    std::uint32_t                                         mSwapchainImageIndex{0U};

    std::array<FrameData, Settings::kMaxFramesInFlight> mFrameData;

    std::array<SubsystemEntry, 9U> mSubsystems{};
    std::size_t                    mInitializedSubsystems{0U};

#if defined(ENABLE_PROFILING)
    tracy::VkCtx* mGpuProfileContext{nullptr};
#endif

    bool mNeedsSwapchainRebuild{false};

    bool mIsInitialized{false};
    bool mIsRecording{false};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_CONTEXT_HPP
