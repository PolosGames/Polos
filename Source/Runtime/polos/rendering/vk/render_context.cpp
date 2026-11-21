//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vk/render_context.hpp"

#include "polos/communication/event_bus.hpp"
#include "polos/gui/backend.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/platform/events/window_framebuffer_resize.hpp"
#include "polos/rendering/cache/material_cache.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/compositor/render_compositor.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_profiling.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace polos::rendering
{

RenderContext* RenderContext::sRenderContext{nullptr};

auto RenderContext::Get() -> RenderContext&
{
    return *sRenderContext;
}

RenderContext::RenderContext()
{
    using platform::WindowFramebufferResize;
    sRenderContext = this;
    communication::Subscribe<WindowFramebufferResize>([this](WindowFramebufferResize&) {
        mHasFramebufferResized = true;
    });
};

RenderContext::~RenderContext()
{
    std::ignore    = Shutdown();
    sRenderContext = nullptr;
}

auto RenderContext::Initialize(platform::IWindowSurface& tSurface) -> Result<void>
{
    if (mIsInitialized)
    {
        return {};
    }

    mWindowSurface    = &tSurface;
    mContext          = std::make_unique<VulkanContext>();
    mDevice           = std::make_unique<VulkanDevice>();
    mSwapchain        = std::make_unique<VulkanSwapchain>();
    mTextureCache     = std::make_unique<TextureCache>();
    mRenderCompositor = std::make_unique<RenderCompositor>();
    mMeshCache        = std::make_unique<MeshCache>();
    mMaterialCache    = std::make_unique<MaterialCache>();

    // Supporting resources share the lifecycle order of the subsystems that use them.
    mSubsystems = {
        RegisterSubsystem(
            *mContext,
            [this] {
                return VulkanContext::CreationDetails{
                    .windowSurface = mWindowSurface,
                };
            }),
        RegisterSubsystem(
            *mDevice,
            [] {
                return VulkanDevice::CreationDetails{
                    .enabledExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
                };
            }),
        SubsystemEntry{
            .createFunc =
                [this] {
                    return createSwapchain(mWindowSurface->GetFramebufferSize());
                },
            .destroyFunc =
                [this] {
                    mSwapchain->Destroy();
                },
        },
        RegisterSubsystem(
            *mTextureCache,
            [] {
                return TextureCache::CreationDetails{};
            }),
        RegisterSubsystem(*mMaterialCache, [] {
            return MaterialCache::CreationDetails{};
        }),
        RegisterSubsystem(
            *mMeshCache,
            [] {
                return MeshCache::CreationDetails{};
            }),
        SubsystemEntry{
            .createFunc =
                [this] {
                    return createFrameResources();
                },
            .destroyFunc =
                [this] {
                    destroyFrameResources();
                },
        },
        RegisterSubsystem(*mRenderCompositor, [] {
            return RenderCompositor::CreationDetails{};
        }),
    };

    if (auto result = CreateSubsystems(mSubsystems, mInitializedSubsystems); !result)
    {
        std::ignore = Shutdown();
        return result;
    }

#if defined(ENABLE_PROFILING)
    std::uint32_t queueCount{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(mDevice->physDevice, &queueCount, nullptr);

    std::vector<VkQueueFamilyProperties> queues(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mDevice->physDevice, &queueCount, queues.data());

    if (queues[mDevice->GetGraphicsQueueFamily()].timestampValidBits > 0U)
    {
        char const* contextName = "Vulkan graphics queue";
        mGpuProfileContext =
            TracyVkContext(mDevice->physDevice, mDevice->logiDevice, mDevice->gfxQueue, mFrameData.front().currentCmdBuf.GetHandle());
        TracyVkContextName(mGpuProfileContext, contextName, std::strlen(contextName));
    }
    else
    {
        LogWarn("The Vulkan graphics queue does not support GPU timestamps. Tracy will record CPU zones.");
    }
#endif

    mIsInitialized = true;
    return {};
}

auto RenderContext::createSwapchain(platform::FramebufferSize const& tFbSize) -> Result<void>
{
    return mSwapchain->Create({
        .fremebufferSize = tFbSize,
        .preferredSurfaceFormats =
            {
                {.format = VK_FORMAT_R8G8B8A8_SRGB, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                {.format = VK_FORMAT_B8G8R8A8_SRGB, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            },
        .preferredPresentModes =
            {
#if defined(ENABLE_PROFILING)
                VK_PRESENT_MODE_IMMEDIATE_KHR,
                VK_PRESENT_MODE_MAILBOX_KHR,
#endif
                VK_PRESENT_MODE_FIFO_KHR,
            },
        .transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    });
}

auto RenderContext::createFrameResources() -> Result<void>
{
    // Create submission semaphores with number of images in swapchain
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html#_discussion_of_solution
    mSubmitSemaphores.resize(mSwapchain->GetImageCount());

    // --- Command buffer allocations ---
    VkCommandBufferAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = mDevice->commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = Settings::kMaxFramesInFlight,
    };

    std::array<VkCommandBuffer, Settings::kMaxFramesInFlight> handles{};
    CHECK_VK_SUCCESS_OR_ERR(
        vkAllocateCommandBuffers(mDevice->logiDevice, &alloc_info, handles.data()),
        RenderingErrc::kFailedCmdBufAlloc);

    for (std::size_t i{0U}; i < handles.size(); ++i)
    {
        mFrameData[i].currentCmdBuf = CommandBuffer{handles[i]};
    }

    // --- Synchronization objects creation ---
    VkSemaphoreCreateInfo const semaphore_info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
    };
    VkFenceCreateInfo const fence_info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (std::uint32_t i{0U}; i < mSwapchain->GetImageCount(); ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(mDevice->logiDevice, &semaphore_info, nullptr, &mSubmitSemaphores[i]),
            RenderingErrc::kFailedCreateSemaphore);
    }

    for (std::size_t i{0U}; i < Settings::kMaxFramesInFlight; ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(mDevice->logiDevice, &semaphore_info, nullptr, &mAcqSemaphores[i]),
            RenderingErrc::kFailedCreateSemaphore);
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateFence(mDevice->logiDevice, &fence_info, nullptr, &mFrameFences[i]),
            RenderingErrc::kFailedCreateFence);
    }

    return {};
}

auto RenderContext::destroyFrameResources() -> void
{
    mFrameData = {};

    for (auto* semaphore : mSubmitSemaphores)
    {
        vkDestroySemaphore(mDevice->logiDevice, semaphore, nullptr);
    }
    mSubmitSemaphores.clear();

    for (std::size_t i{0U}; i < Settings::kMaxFramesInFlight; ++i)
    {
        vkDestroySemaphore(mDevice->logiDevice, mAcqSemaphores[i], nullptr);
        vkDestroyFence(mDevice->logiDevice, mFrameFences[i], nullptr);
    }
    mAcqSemaphores.fill(VK_NULL_HANDLE);
    mFrameFences.fill(VK_NULL_HANDLE);
}

auto RenderContext::BeginFrame() -> CommandBuffer*
{
    POLOS_PROFILE_ZONE("Begin frame");
    {
        POLOS_PROFILE_ZONE("Wait for frame fence");
        vkWaitForFences(
            mDevice->logiDevice,
            1U,
            &mFrameFences[mCurrentFrameIndex],
            VK_TRUE,
            std::numeric_limits<std::uint64_t>::max());
    }

    bool acq_ok{false};
    while (!acq_ok)
    {
        AcquireNextImageDetails const next_img_dets{
            .semaphore = mAcqSemaphores[mCurrentFrameIndex],
            .fence     = VK_NULL_HANDLE,
            .timeout   = std::numeric_limits<std::uint64_t>::max(),
        };

        auto const acq_result = mSwapchain->AcquireNextImage(next_img_dets);
        if (VulkanSwapchain::kInvalidImageIndex != acq_result)
        {
            acq_ok = true;
            break;
        }
        onFramebufferResize();
    }

    mSwapchainImageIndex = mSwapchain->GetCurrentImageIndex();

    auto& frame  = mFrameData[mCurrentFrameIndex];
    auto& cmdBuf = frame.currentCmdBuf;

    VkCommandBuffer handle = cmdBuf.GetHandle();
    VkCommandBufferBeginInfo const begin_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .pInheritanceInfo = nullptr,
    };

    vkResetCommandBuffer(handle, 0U);
    PL_ASSERT(
        vkBeginCommandBuffer(handle, &begin_info) == VK_SUCCESS,
        "Could not begin recording the frame command buffer.");

#if defined(ENABLE_PROFILING)
    if (mGpuProfileContext != nullptr)
    {
        TracyVkCollect(mGpuProfileContext, handle);
    }
#endif

    mIsRecording = true;

    frame.frameSlot    = mCurrentFrameIndex;
    frame.scImageIndex = mSwapchainImageIndex;
    frame.scImage      = mSwapchain->GetCurrentImageResource();
    frame.scImageView  = mSwapchain->GetCurrentImageView();

    mRenderCompositor->Prepare(frame);
    {
        POLOS_PROFILE_ZONE("Prepare ImGui frame");
        gui::backend::BeginFrame();
    }

    return &cmdBuf;
}

auto RenderContext::EndFrame() -> FrameStatistics
{
    POLOS_PROFILE_ZONE("End frame");
    FrameStatistics statistics;
    statistics.drawCalls = mRenderCompositor->Record(mFrameData[mCurrentFrameIndex]);

    VkCommandBuffer handle = mFrameData[mCurrentFrameIndex].currentCmdBuf.GetHandle();
    PL_ASSERT(
        vkEndCommandBuffer(handle) == VK_SUCCESS,
        "Could not finish recording the frame command buffer.");

    std::array<VkPipelineStageFlags, 1> const wait_stages = {VK_PIPELINE_STAGE_TRANSFER_BIT};

    VkSubmitInfo const submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1U,
        .pWaitSemaphores      = &mAcqSemaphores[mCurrentFrameIndex],
        .pWaitDstStageMask    = wait_stages.data(),
        .commandBufferCount   = 1U,
        .pCommandBuffers      = &handle,
        .signalSemaphoreCount = 1U,
        .pSignalSemaphores    = &mSubmitSemaphores[mSwapchainImageIndex],
    };

    {
        POLOS_PROFILE_ZONE("Submit frame to graphics queue");
        vkResetFences(mDevice->logiDevice, 1U, &mFrameFences[mCurrentFrameIndex]);
        PL_ASSERT(
            vkQueueSubmit(mDevice->gfxQueue, 1U, &submit_info, mFrameFences[mCurrentFrameIndex]) == VK_SUCCESS,
            "Could not submit the frame command buffer to the graphics queue.");
    }
    mIsRecording = false;

    if (!mSwapchain->QueuePresent(mSubmitSemaphores[mSwapchainImageIndex]) || mHasFramebufferResized)
    {
        mHasFramebufferResized = false;
        onFramebufferResize();
    }

    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % Settings::kMaxFramesInFlight;

    return statistics;
}

auto RenderContext::BeginSingleTimeCommands() -> CommandBuffer
{
    VkCommandBufferAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = mDevice->commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1U,
    };

    VkCommandBuffer handle{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkAllocateCommandBuffers(mDevice->logiDevice, &alloc_info, &handle))
    {
        LogError("Could not allocate command buffer for single time commands!");
        return {};
    }

    VkCommandBufferBeginInfo const begin_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    if (VK_SUCCESS != vkBeginCommandBuffer(handle, &begin_info))
    {
        LogError("Could not begin command buffer for single time commands!");
        vkFreeCommandBuffers(
            mDevice->logiDevice,
            mDevice->commandPool,
            1U,
            &handle);
        return {};
    }

    return CommandBuffer{handle};
}

auto RenderContext::EndSingleTimeCommands(CommandBuffer& tCmdBuf) -> bool
{
    VkCommandBuffer handle = tCmdBuf.GetHandle();
    tCmdBuf = {};

    if (VK_SUCCESS != vkEndCommandBuffer(handle))
    {
        LogError("Could not end single-time command buffer!");
        vkFreeCommandBuffers(
            mDevice->logiDevice,
            mDevice->commandPool,
            1U,
            &handle);
        return false;
    }

    VkSubmitInfo const submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0U,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1U,
        .pCommandBuffers      = &handle,
        .signalSemaphoreCount = 0U,
        .pSignalSemaphores    = nullptr,
    };

    if (VK_SUCCESS != vkQueueSubmit(mDevice->gfxQueue, 1U, &submit_info, VK_NULL_HANDLE))
    {
        LogError("Could not submit single-time command buffer to queue!");
        vkFreeCommandBuffers(
            mDevice->logiDevice,
            mDevice->commandPool,
            1U,
            &handle);
        return false;
    }
    auto const waitResult = vkQueueWaitIdle(mDevice->gfxQueue);

    vkFreeCommandBuffers(
        mDevice->logiDevice,
        mDevice->commandPool,
        1U,
        &handle);
    if (waitResult != VK_SUCCESS)
    {
        LogError("Could not wait for single-time commands to complete!");
        return false;
    }
    return true;
}

auto RenderContext::IsInitialized() -> bool
{
    return mIsInitialized;
}


#if defined(ENABLE_PROFILING)
auto RenderContext::GetGpuProfileContext() -> tracy::VkCtx*
{
    return mGpuProfileContext;
}
#endif

void RenderContext::onFramebufferResize()
{
    POLOS_PROFILE_ZONE("Recreate swapchain");
    vkDeviceWaitIdle(mDevice->logiDevice);
    LogTrace("Recreating swapchain due to framebuffer resize...");

    mSwapchain->Destroy();
    vkDeviceWaitIdle(mDevice->logiDevice);

    {
        auto const res = createSwapchain(mWindowSurface->GetFramebufferSize());
        PL_ASSERT(res.has_value(), "Failed to recreate the swapchain after destroying the old swapchain.");
    }

    {
        auto const res = mRenderCompositor->RecreateTargets();
        PL_ASSERT(res.has_value(), "Failed to recreate render targets after releasing the old targets.");
    }
}

auto RenderContext::Shutdown() -> Result<void>
{
    if (mDevice && mDevice->logiDevice != VK_NULL_HANDLE)
    {
        LogInfo("Waiting for device idle to destroy Vulkan RenderContext...");
        vkDeviceWaitIdle(mDevice->logiDevice);
    }

    if (mIsRecording)
    {
        vkEndCommandBuffer(mFrameData[mCurrentFrameIndex].currentCmdBuf.GetHandle());
        mIsRecording = false;
    }

#if defined(ENABLE_PROFILING)
    if (mGpuProfileContext != nullptr)
    {
        // The device is idle here, so all remaining timestamp results can be sent to Tracy.
        if (auto cmdBuf = BeginSingleTimeCommands(); cmdBuf)
        {
            TracyVkCollect(mGpuProfileContext, cmdBuf.GetHandle());
            std::ignore = EndSingleTimeCommands(cmdBuf);
        }
        TracyVkDestroy(mGpuProfileContext);
        mGpuProfileContext = nullptr;
    }
#endif

    DestroySubsystems(mSubsystems, mInitializedSubsystems);
    mSubsystems = {};

    mRenderCompositor.reset();
    mMaterialCache.reset();
    mTextureCache.reset();
    mMeshCache.reset();
    mSwapchain.reset();
    mDevice.reset();
    mContext.reset();
    mWindowSurface         = nullptr;
    mIsInitialized         = false;
    mHasFramebufferResized = false;
    mCurrentFrameIndex     = 0U;
    mSwapchainImageIndex   = 0U;

    return {};
}

}// namespace polos::rendering
