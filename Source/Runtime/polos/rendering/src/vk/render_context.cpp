//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "vk/render_context.hpp"

#include "cache/pipeline_cache.hpp"
#include "cache/shader_cache.hpp"
#include "compositor/render_compositor.hpp"
#include "polos/communication/error_code.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_framebuffer_resize.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/utils/build_info.hpp"
#include "resources/gpu_buffer.hpp"
#include "resources/gpu_image.hpp"
#include "vk/common.hpp"
#include "vk/vulkan_context.hpp"
#include "vk/vulkan_device.hpp"
#include "vk/vulkan_swapchain.hpp"

#include <vulkan/vulkan.h>

#include <array>

#define INIT_VULKAN_COMPONENT(SubmodulePtr, ...)         \
    {                                                    \
        auto result = SubmodulePtr->Create(__VA_ARGS__); \
        if (!result.has_value())                         \
        {                                                \
            return ErrorType{result.error()};            \
        }                                                \
    }

namespace polos::rendering
{
namespace
{

auto FindQueueFamily(VkPhysicalDevice tDevice, VkSurfaceKHR tSurface, VkQueueFlags tFlags) -> Result<std::uint32_t>
{
    std::uint32_t queue_family_count{0U};
    vkGetPhysicalDeviceQueueFamilyProperties(tDevice, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families{queue_family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(tDevice, &queue_family_count, queue_families.data());

    std::uint32_t queue_index{0U};
    auto const    queue_families_size = static_cast<std::uint32_t>(queue_families.size());
    for (; queue_index < queue_families_size; ++queue_index)
    {
        VkQueueFamilyProperties const& family = queue_families[queue_index];

        if (0U != (family.queueFlags & tFlags))
        {
            if (0U != (tFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                // If gfx family is requested, also check if it has present capabilities
                VkBool32 present_support{0U};
                vkGetPhysicalDeviceSurfaceSupportKHR(tDevice, queue_index, tSurface, &present_support);

                if (0U != present_support)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (queue_index == VK_SIZE_CAST(queue_families.size()))
    {
        return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
    }

    return queue_index;
}

}// namespace

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
{ sRenderContext = nullptr; }

auto RenderContext::Initialize(IWindowSurface& tSurface) -> Result<void>
{
    mWindowSurface    = &tSurface;
    mContext          = std::make_unique<VulkanContext>();
    mDevice           = std::make_unique<VulkanDevice>();
    mSwapchain        = std::make_unique<VulkanSwapchain>();
    mShaderCache      = std::make_unique<ShaderCache>();
    mPipelineCache    = std::make_unique<PipelineCache>();
    mRenderCompositor = std::make_unique<RenderCompositor>();

    // --- Initialize Vulkan context ---
    ContextCreateDetails const context_details{
        .requiredExtensions = tSurface.RequiredInstanceExtensions(),
    };
    INIT_VULKAN_COMPONENT(mContext, context_details);

    // --- Surface Creation ---
    auto surface = tSurface.CreateSurface(mContext->mInstance);
    if (!surface.has_value())
    {
        return ErrorType{surface.error()};
    }
    mSurface = *surface;

    // --- Physical device selection ---
    std::uint32_t device_count{0U};
    vkEnumeratePhysicalDevices(mContext->mInstance, &device_count, nullptr);
    if (0U == device_count)
    {
        return ErrorType{RenderingErrc::kFailedFindPhysDevice};
    }

    std::vector<VkPhysicalDevice> physical_devices(static_cast<std::size_t>(device_count));
    vkEnumeratePhysicalDevices(mContext->mInstance, &device_count, physical_devices.data());

    // TODO(sorbatdev): Select the first available GPU, no need for suitability checks for now.
    VkPhysicalDevice phys_device = physical_devices[0];

    // --- Suitable queue family selection ---
    {// Graphics Family
        auto const q_result = FindQueueFamily(phys_device, mSurface, VK_QUEUE_GRAPHICS_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }
        mQueueFamilyIndices.gfxQIndex = q_result.value();
    }
    {// Transfer Family
        auto const q_result = FindQueueFamily(phys_device, mSurface, VK_QUEUE_TRANSFER_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }
        mQueueFamilyIndices.transferQIndex = q_result.value();
    }
    {// Compute Family
        auto const q_result = FindQueueFamily(phys_device, mSurface, VK_QUEUE_COMPUTE_BIT);
        if (!q_result.has_value())
        {
            return ErrorType{RenderingErrc::kNoAdequateQueueFamily};
        }

        mQueueFamilyIndices.computeQIndex = q_result.value();
    }

    // --- Device creation ---
    {
        DeviceCreateDetails const info{
            .instance          = mContext->mInstance,
            .surface           = mSurface,
            .physDevice        = phys_device,
            .qIndices          = mQueueFamilyIndices,
            .enabledExtensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME}},
        };
        INIT_VULKAN_COMPONENT(mDevice, info);
    }

    GpuImage::sAllocator  = mDevice->GetAllocator();
    GpuBuffer::sAllocator = mDevice->GetAllocator();

    // Get the graphics queue for now
    vkGetDeviceQueue(mDevice->mLogiDevice, mQueueFamilyIndices.gfxQIndex, 0U, &mGfxQueue);

    // --- Swapchain creation ---
    {
        SwapchainCreateDetails const details{
            .device        = mDevice.get(),
            .physDevice    = phys_device,
            .surface       = mSurface,
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
            .gfxQueue       = mGfxQueue,
            .transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        };

        INIT_VULKAN_COMPONENT(mSwapchain, details);
    }

    {
        PipelineCacheCreateDetails const details{
            .logiDevice = mDevice->mLogiDevice,
            .swapchain  = mSwapchain.get(),
        };

        INIT_VULKAN_COMPONENT(mPipelineCache, details);
    }

    {
        ShaderCacheCreateDetails const details{
            .logiDevice  = mDevice->mLogiDevice,
            .shaderFiles = {
                {
                    .customName = "s_basiccolor_vt",
                    .stage      = ShaderStage::kVertex,
                    .path       = "Resource/Shaders/basic_color.vert.spv",
                },
                {
                    .customName = "s_basiccolor_fm",
                    .stage      = ShaderStage::kFragment,
                    .path       = "Resource/Shaders/basic_color.frag.spv",
                },
            },
        };

        INIT_VULKAN_COMPONENT(mShaderCache, details);
    }

    // --- Command Pool creation ---
    VkCommandPoolCreateInfo const pool_info{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = mQueueFamilyIndices.gfxQIndex,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkCreateCommandPool(mDevice->mLogiDevice, &pool_info, nullptr, &mCommandPool),
        RenderingErrc::kFailedCreateCmdPool);

    // Create submission semaphores with number of images in swapchain
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html#_discussion_of_solution
    mSubmitSemaphores.resize(mSwapchain->GetImageCount());

    // --- Command buffer allocations ---
    VkCommandBufferAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = mCommandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = Settings::kMaxFramesInFlight,
    };

    CHECK_VK_SUCCESS_OR_ERR(
        vkAllocateCommandBuffers(mDevice->mLogiDevice, &alloc_info, mFrameCommandBuffers.data()),
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
            vkCreateSemaphore(mDevice->mLogiDevice, &semaphore_info, nullptr, &mSubmitSemaphores[i]),
            RenderingErrc::kFailedCreateSemaphore);
    }

    for (std::size_t i{0U}; i < Settings::kMaxFramesInFlight; ++i)
    {
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateSemaphore(mDevice->mLogiDevice, &semaphore_info, nullptr, &mAcqSemaphores[i]),
            RenderingErrc::kFailedCreateSemaphore);
        CHECK_VK_SUCCESS_OR_ERR(
            vkCreateFence(mDevice->mLogiDevice, &fence_info, nullptr, &mFrameFences[i]),
            RenderingErrc::kFailedCreateFence);
    }

    // --- Render Compositor
    {
        RenderCompositorCreateDetails const details{
            .context    = *this,
            .scExtent   = mSwapchain->GetExtent(),
            .logiDevice = mDevice->mLogiDevice,
        };
        INIT_VULKAN_COMPONENT(mRenderCompositor, details);
    }

    mIsInitialized = true;

    return {};
}

auto RenderContext::BeginFrame() -> VkCommandBuffer
{
    vkWaitForFences(
        mDevice->mLogiDevice,
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
        if (acq_result.has_value())
        {
            acq_ok = true;
            break;
        }
        onFramebufferResize();
        LogWarn("{}", acq_result.error());
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
    if (VK_SUCCESS != vkBeginCommandBuffer(cur_cmd_buf, &begin_info))
    {
        LogCritical("Could not begin recording command buffer!");
        return nullptr;
    }

    isRecording = true;

    mFrameData[mCurrentFrameIndex] = FrameData{
        .frameSlot     = mCurrentFrameIndex,
        .scImageIndex  = mSwapchainImageIndex,
        .currentCmdBuf = cur_cmd_buf,
        .scImage       = mSwapchain->GetCurrentImage(),
        .scImageView   = mSwapchain->GetCurrentImageView(),
        .scExtent      = mSwapchain->GetExtent(),
    };

    std::ignore = mRenderCompositor->Prepare(mFrameData[mCurrentFrameIndex]);

    return cur_cmd_buf;
}

auto RenderContext::renderFrame(SceneData const& tView) -> void
{ mRenderCompositor->Record(mFrameData[mCurrentFrameIndex], tView); }

auto RenderContext::EndFrame(SceneData const& tView) -> void
{
    renderFrame(tView);

    if (VK_SUCCESS != vkEndCommandBuffer(mFrameCommandBuffers[mCurrentFrameIndex]))
    {
        LogCritical("Could not record command buffer.");
        return;
    }

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

    vkResetFences(mDevice->mLogiDevice, 1U, &mFrameFences[mCurrentFrameIndex]);
    if (VK_SUCCESS != vkQueueSubmit(mGfxQueue, 1U, &submit_info, mFrameFences[mCurrentFrameIndex]))
    {
        LogCritical("Could not submit draw command buffer to the graphics queue!");
        return;
    }
    isRecording = false;

    auto result = mSwapchain->QueuePresent(mSubmitSemaphores[mSwapchainImageIndex]);
    if (!result.has_value())
    {
        onFramebufferResize();
        LogInfo("{}", result.error());
    }

    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % Settings::kMaxFramesInFlight;
}

auto RenderContext::GetShaderCache() const -> ShaderCache&
{ return *mShaderCache; }

auto RenderContext::GetPipelineCache() const -> PipelineCache&
{ return *mPipelineCache; }

VkCommandBuffer RenderContext::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = sRenderContext->mCommandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1U,
    };

    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkAllocateCommandBuffers(sRenderContext->mDevice->mLogiDevice, &alloc_info, &command_buffer))
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
        return VK_NULL_HANDLE;
    }

    return command_buffer;
}

void RenderContext::EndSingleTimeCommands(VkCommandBuffer tCommandBuffer)
{
    if (VK_SUCCESS != vkEndCommandBuffer(tCommandBuffer))
    {
        LogError("Could not end single-time command buffer!");
        vkFreeCommandBuffers(sRenderContext->mDevice->mLogiDevice, sRenderContext->mCommandPool, 1U, &tCommandBuffer);
        return;
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

    if (VK_SUCCESS != vkQueueSubmit(sRenderContext->mGfxQueue, 1U, &submit_info, VK_NULL_HANDLE))
    {
        LogError("Could not submit single-time command buffer to queue!");
        vkFreeCommandBuffers(sRenderContext->mDevice->mLogiDevice, sRenderContext->mCommandPool, 1U, &tCommandBuffer);
        return;
    }
    vkQueueWaitIdle(sRenderContext->mGfxQueue);

    vkFreeCommandBuffers(sRenderContext->mDevice->mLogiDevice, sRenderContext->mCommandPool, 1U, &tCommandBuffer);
}

auto RenderContext::IsInitialized() const -> bool
{ return mIsInitialized; }

auto RenderContext::GetVkSurface() -> VkSurfaceKHR
{ return mSurface; }

auto RenderContext::GetGfxQueue() -> VkQueue
{ return mGfxQueue; }

auto RenderContext::GetSwapchain() -> VulkanSwapchain&
{ return *mSwapchain; }

auto RenderContext::GetVulkanDevice() -> VulkanDevice&
{ return *mDevice; }

auto RenderContext::GetCommandPool() -> VkCommandPool
{ return mCommandPool; }

void RenderContext::onFramebufferResize()
{
    vkDeviceWaitIdle(mDevice->mLogiDevice);
    LogInfo("Recreating swapchain due to framebuffer resize...");

    std::ignore = mSwapchain->Destroy();
    SwapchainCreateDetails const details{
        .device        = mDevice.get(),
        .physDevice    = mDevice->mPhysDevice,
        .surface       = mSurface,
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
        .gfxQueue       = mGfxQueue,
        .transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    };
    vkDeviceWaitIdle(mDevice->mLogiDevice);

    if (auto const res = mSwapchain->Create(details); !res.has_value())
    {
        LogCritical("{}", res.error());
    }
}

auto RenderContext::Shutdown() -> Result<void>
{
    if (!mIsInitialized)
    {
        return {};
    }

    LogInfo("Waiting for device idle to destroy Vulkan RenderContext...");

    vkDeviceWaitIdle(mDevice->mLogiDevice);

    LogInfo("Destroying Vulkan resources...");

    for (std::uint32_t i{0U}; i < mSwapchain->GetImageCount(); ++i)
    {
        vkDestroySemaphore(mDevice->mLogiDevice, mSubmitSemaphores[i], nullptr);
    }

    for (std::size_t i{0U}; i < Settings::kMaxFramesInFlight; ++i)
    {
        vkDestroySemaphore(mDevice->mLogiDevice, mAcqSemaphores[i], nullptr);
        vkDestroyFence(mDevice->mLogiDevice, mFrameFences[i], nullptr);
    }

    VkCommandBuffer cur_cmd_buf = mFrameCommandBuffers[mCurrentFrameIndex];
    if (isRecording)
    {
        vkEndCommandBuffer(cur_cmd_buf);
    }
    for (auto* buf : mFrameCommandBuffers) { vkResetCommandBuffer(buf, 0U); }
    vkDestroyCommandPool(mDevice->mLogiDevice, mCommandPool, nullptr);

    mRenderCompositor->Destroy();
    std::ignore = mPipelineCache->Destroy();
    std::ignore = mShaderCache->Destroy();
    std::ignore = mSwapchain->Destroy();
    std::ignore = mDevice->Destroy();
    vkDestroySurfaceKHR(mContext->mInstance, mSurface, nullptr);
    std::ignore = mContext->Destroy();

    LogInfo("Vulkan destroy complete!");

    return {};
}

}// namespace polos::rendering

polos::rendering::IRenderContext* CreateRenderContext()
{
    // reports the module's own build, not the host's - a reload that logs an unchanged number
    // means the .so on disk was never rebuilt
    LogInfo("rendering_impl build {}", polos::utils::FormatBuildInfo(polos::utils::GetBuildInfo()));

    // Capsulated in PlatformManager
    return new polos::rendering::RenderContext{};// NOLINT
}
