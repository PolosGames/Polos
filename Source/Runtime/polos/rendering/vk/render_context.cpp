//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vk/render_context.hpp"

#include "polos/gui/backend.hpp"

#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_framebuffer_resize.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/cache/pipeline_cache.hpp"
#include "polos/rendering/cache/shader_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/compositor/render_compositor.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <array>

namespace polos::rendering
{

RenderContext* RenderContext::sRenderContext{nullptr};

RenderContext::RenderContext()
{
    using communication::WindowFramebufferResize;
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
    mShaderCache      = std::make_unique<ShaderCache>();
    mPipelineCache    = std::make_unique<PipelineCache>();
    mTextureCache     = std::make_unique<TextureCache>();
    mRenderCompositor = std::make_unique<RenderCompositor>();

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
                    .enabledExtensions =
                        {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME},
                };
            }),
        RegisterSubsystem(
            *mSwapchain,
            [this] {
                return VulkanSwapchain::CreationDetails{
                    .windowSurface = mWindowSurface,
                    .preferredSurfaceFormats =
                        {
                            {
                                .format     = VK_FORMAT_B8G8R8A8_SRGB,
                                .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                            },
                            {
                                .format     = VK_FORMAT_R8G8B8A8_SRGB,
                                .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                            },
                        },
                    .preferredPresentModes =
                        {
                            VK_PRESENT_MODE_MAILBOX_KHR,
                            VK_PRESENT_MODE_FIFO_KHR,
                        },
                    .transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                };
            }),
        RegisterSubsystem(
            *mPipelineCache,
            [] {
                return PipelineCache::CreationDetails{};
            }),
        RegisterSubsystem(
            *mShaderCache,
            [] {
                return ShaderCache::CreationDetails{
                    .shaderFiles = {
                        {
                            .stage      = ShaderStage::kVertex,
                            .resourceId = "shader.quad.vertex"_sid,
                        },
                        {
                            .stage      = ShaderStage::kFragment,
                            .resourceId = "shader.quad.fragment"_sid,
                        },
                    },
                };
            }),
        RegisterSubsystem(
            *mTextureCache,
            [] {
                return TextureCache::CreationDetails{};
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
        RegisterSubsystem(*mRenderCompositor, [this] {
            return RenderCompositor::CreationDetails{
                .context = *this,
            };
        }),
    };

    if (auto result = CreateSubsystems(mSubsystems, mInitializedSubsystems); !result)
    {
        std::ignore = Shutdown();
        return result;
    }

    mIsInitialized = true;
    return {};
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

    CHECK_VK_SUCCESS_OR_ERR(
        vkAllocateCommandBuffers(mDevice->logiDevice, &alloc_info, mFrameCommandBuffers.data()),
        RenderingErrc::kFailedCmdBufAlloc);

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
    mFrameCommandBuffers.fill(VK_NULL_HANDLE);
}

auto RenderContext::BeginFrame() -> VkCommandBuffer
{
    vkWaitForFences(
        mDevice->logiDevice,
        1U,
        &mFrameFences[mCurrentFrameIndex],
        VK_TRUE,
        std::numeric_limits<std::uint64_t>::max());

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

    VkCommandBuffer                cur_cmd_buf = mFrameCommandBuffers[mCurrentFrameIndex];
    VkCommandBufferBeginInfo const begin_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .pInheritanceInfo = nullptr,
    };

    vkResetCommandBuffer(cur_cmd_buf, 0U);
    PL_ASSERT(
        vkBeginCommandBuffer(cur_cmd_buf, &begin_info) == VK_SUCCESS,
        "Could not begin recording the frame command buffer.");

    mIsRecording = true;

    mFrameData[mCurrentFrameIndex] = FrameData{
        .frameSlot     = mCurrentFrameIndex,
        .scImageIndex  = mSwapchainImageIndex,
        .currentCmdBuf = cur_cmd_buf,
        .scImage       = mSwapchain->GetCurrentImageResource(),
        .scImageView   = mSwapchain->GetCurrentImageView(),
    };

    mRenderCompositor->Prepare(mFrameData[mCurrentFrameIndex]);
    gui::backend::BeginFrame();

    return cur_cmd_buf;
}

auto RenderContext::EndFrame() -> void
{
    mRenderCompositor->Record(mFrameData[mCurrentFrameIndex]);

    PL_ASSERT(
        vkEndCommandBuffer(mFrameCommandBuffers[mCurrentFrameIndex]) == VK_SUCCESS,
        "Could not finish recording the frame command buffer.");

    std::array<VkPipelineStageFlags, 1> const wait_stages = {VK_PIPELINE_STAGE_TRANSFER_BIT};

    VkSubmitInfo const submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1U,
        .pWaitSemaphores      = &mAcqSemaphores[mCurrentFrameIndex],
        .pWaitDstStageMask    = wait_stages.data(),
        .commandBufferCount   = 1U,
        .pCommandBuffers      = &mFrameCommandBuffers[mCurrentFrameIndex],
        .signalSemaphoreCount = 1U,
        .pSignalSemaphores    = &mSubmitSemaphores[mSwapchainImageIndex],
    };

    vkResetFences(mDevice->logiDevice, 1U, &mFrameFences[mCurrentFrameIndex]);
    PL_ASSERT(
        vkQueueSubmit(mDevice->gfxQueue, 1U, &submit_info, mFrameFences[mCurrentFrameIndex]) == VK_SUCCESS,
        "Could not submit the frame command buffer to the graphics queue.");
    mIsRecording = false;

    if (!mSwapchain->QueuePresent(mSubmitSemaphores[mSwapchainImageIndex]))
    {
        LogWarn("Failed to present the image to the presentation queue. Recreating the swapchain.");
        onFramebufferResize();
    }

    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % Settings::kMaxFramesInFlight;
}

auto RenderContext::GetShaderCache() -> ShaderCache&
{
    return *sRenderContext->mShaderCache;
}

auto RenderContext::GetPipelineCache() -> PipelineCache&
{
    return *sRenderContext->mPipelineCache;
}

VkCommandBuffer RenderContext::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = RenderContext::GetVulkanDevice().commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1U,
    };

    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkAllocateCommandBuffers(sRenderContext->mDevice->logiDevice, &alloc_info, &command_buffer))
    {
        LogError("Could not allocate command buffer for single time commands!");
        return VK_NULL_HANDLE;
    }

    VkCommandBufferBeginInfo const begin_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    if (VK_SUCCESS != vkBeginCommandBuffer(command_buffer, &begin_info))
    {
        LogError("Could not begin command buffer for single time commands!");
        vkFreeCommandBuffers(
            sRenderContext->mDevice->logiDevice,
            RenderContext::GetVulkanDevice().commandPool,
            1U,
            &command_buffer);
        return VK_NULL_HANDLE;
    }

    return command_buffer;
}

bool RenderContext::EndSingleTimeCommands(VkCommandBuffer tCommandBuffer)
{
    if (VK_SUCCESS != vkEndCommandBuffer(tCommandBuffer))
    {
        LogError("Could not end single-time command buffer!");
        vkFreeCommandBuffers(
            sRenderContext->mDevice->logiDevice,
            RenderContext::GetVulkanDevice().commandPool,
            1U,
            &tCommandBuffer);
        return false;
    }

    VkSubmitInfo const submit_info{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0U,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1U,
        .pCommandBuffers      = &tCommandBuffer,
        .signalSemaphoreCount = 0U,
        .pSignalSemaphores    = nullptr,
    };

    if (VK_SUCCESS != vkQueueSubmit(RenderContext::GetVulkanDevice().gfxQueue, 1U, &submit_info, VK_NULL_HANDLE))
    {
        LogError("Could not submit single-time command buffer to queue!");
        vkFreeCommandBuffers(
            sRenderContext->mDevice->logiDevice,
            RenderContext::GetVulkanDevice().commandPool,
            1U,
            &tCommandBuffer);
        return false;
    }
    auto const waitResult = vkQueueWaitIdle(RenderContext::GetVulkanDevice().gfxQueue);

    vkFreeCommandBuffers(
        sRenderContext->mDevice->logiDevice,
        RenderContext::GetVulkanDevice().commandPool,
        1U,
        &tCommandBuffer);
    if (waitResult != VK_SUCCESS)
    {
        LogError("Could not wait for single-time commands to complete!");
        return false;
    }
    return true;
}

auto RenderContext::IsInitialized() -> bool
{
    return sRenderContext->mIsInitialized;
}

auto RenderContext::GetVulkanContext() -> VulkanContext&
{
    return *sRenderContext->mContext;
}

auto RenderContext::GetVulkanDevice() -> VulkanDevice&
{
    return *sRenderContext->mDevice;
}

auto RenderContext::GetSwapchain() -> VulkanSwapchain&
{
    return *sRenderContext->mSwapchain;
}


void RenderContext::onFramebufferResize()
{
    vkDeviceWaitIdle(mDevice->logiDevice);
    LogInfo("Recreating swapchain due to framebuffer resize...");

    mSwapchain->Destroy();
    VulkanSwapchain::CreationDetails const details{
        .windowSurface = mWindowSurface,
        .preferredSurfaceFormats =
            {
                {
                    .format     = VK_FORMAT_B8G8R8A8_SRGB,
                    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                },
                {
                    .format     = VK_FORMAT_R8G8B8A8_SRGB,
                    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                },
            },
        .preferredPresentModes =
            {
                VK_PRESENT_MODE_MAILBOX_KHR,
                VK_PRESENT_MODE_FIFO_KHR,
            },
        .transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    };
    vkDeviceWaitIdle(mDevice->logiDevice);

    {
        auto const res = mSwapchain->Create(details);
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
        vkEndCommandBuffer(mFrameCommandBuffers[mCurrentFrameIndex]);
        mIsRecording = false;
    }

    DestroySubsystems(mSubsystems, mInitializedSubsystems);
    mSubsystems = {};

    mRenderCompositor.reset();
    mTextureCache.reset();
    mShaderCache.reset();
    mPipelineCache.reset();
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
