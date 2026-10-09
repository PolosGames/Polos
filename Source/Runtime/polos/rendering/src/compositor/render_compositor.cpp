///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "compositor/render_compositor.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "resources/image_description.hpp"
#include "scene/scene_data.hpp"
#include "vk/frame_data.hpp"
#include "vk/render_context.hpp"
#include "vk/vulkan_device.hpp"
#include "vk/vulkan_swapchain.hpp"
#include "vk/vulkan_util.hpp"

#include <utility>

namespace polos::rendering
{

RenderCompositor::~RenderCompositor() = default;

auto RenderCompositor::Create(RenderCompositorCreateDetails const& tDetails) -> Result<void>
{
    mLastExtent  = tDetails.scExtent;
    mDevice      = tDetails.logiDevice;
    mGeneralPass = std::make_unique<GeneralPass>(tDetails.context);

    if (auto res = mGeneralPass->Initialize(); !res.has_value())
    {
        return ErrorType{res.error()};
    }

    if (auto res = createTargets(); !res.has_value())
    {
        return ErrorType{res.error()};
    }

    return {};
}

auto RenderCompositor::Destroy() -> void
{
    mGeneralPass.reset();
    releaseTargets();
}

auto RenderCompositor::Prepare(FrameData const& tFrameData) -> Result<void>
{
    if (tFrameData.scImage->extent != mLastExtent)
    {
        vkDeviceWaitIdle(mDevice);
        for (std::uint32_t slot = 0U; slot < Settings::kMaxFramesInFlight; ++slot) { mGeneralPass->Prepare(slot); }
        mLastExtent = tFrameData.scImage->extent;
        releaseTargets();
        if (auto res = createTargets(); !res.has_value())
        {
            return ErrorType{res.error()};
        }
    }

    return {};
}

auto RenderCompositor::Record(FrameData const& tFrameData, SceneData const& tSceneData) -> void
{
    FrameTargets& currentTarget = mFrameTargets[tFrameData.frameSlot];

    mGeneralPass->Record(tFrameData, tSceneData, currentTarget);

    if (auto result = tFrameData.scImage->ChangeLayout(tFrameData.currentCmdBuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        !result.has_value())
    {
        LogError("{}", result.error());
        return;
    }

    tFrameData.scImage->BlitFrom(tFrameData.currentCmdBuf, *currentTarget.colorImg);

    currentTarget.colorImg->SetUse(ImageUse::ForLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL));
    if (auto result = tFrameData.scImage->ChangeLayout(tFrameData.currentCmdBuf, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        !result.has_value())
    {
        LogError("{}", result.error());
    }
}

auto RenderCompositor::createTargets() -> Result<void>
{
    if (!createColorTarget())
    {
        return ErrorType{RenderingErrc::kFailedCreateColorTarget};
    }
    if (!createDepthTarget())
    {
        return ErrorType{RenderingErrc::kFailedCreateDepthTarget};
    }

    return {};
}

auto RenderCompositor::releaseTargets() -> void
{
    for (auto& target : mFrameTargets)
    {
        if (target.colorImgView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(mDevice, target.colorImgView, nullptr);
            target.colorImgView = VK_NULL_HANDLE;
        }
        if (target.depthImgView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(mDevice, target.depthImgView, nullptr);
            target.depthImgView = VK_NULL_HANDLE;
        }
        target.colorImg.reset();
        target.depthImg.reset();
    }
}

auto RenderCompositor::createColorTarget() -> bool
{
    VkImageUsageFlags usage{0U};
    usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    ImageDescription colorDesc{
        .extent =
            {
                .width  = mLastExtent.width,
                .height = mLastExtent.height,
                .depth  = 1U,
            },
        .format      = VK_FORMAT_R16G16B16A16_SFLOAT,
        .usage       = usage,
        .mipLevels   = 1U,
        .arrayLayers = 1U,
        .samples     = VK_SAMPLE_COUNT_1_BIT,
    };

    for (auto& targets : mFrameTargets)
    {
        auto idxRes = GpuImage::Create(colorDesc);
        if (!idxRes.has_value())
        {
            LogError("{}, [Color Image was not created.]", idxRes.error());
            return false;
        }
        targets.colorImg     = std::move(*idxRes);
        targets.colorImgView = util::CreateImageView(
            mDevice,
            targets.colorImg->img,
            VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_ASPECT_COLOR_BIT);

        if (VK_NULL_HANDLE == targets.colorImgView)
        {
            return false;
        }
    }

    return true;
}

auto RenderCompositor::createDepthTarget() -> bool
{
    ImageDescription depthDesc{
        .extent =
            {
                .width  = mLastExtent.width,
                .height = mLastExtent.height,
                .depth  = 1U,
            },
        .format      = VK_FORMAT_D32_SFLOAT,
        .usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .mipLevels   = 1U,
        .arrayLayers = 1U,
        .samples     = VK_SAMPLE_COUNT_1_BIT,
    };

    for (auto& target : mFrameTargets)
    {
        auto idxRes = GpuImage::Create(depthDesc);
        if (!idxRes.has_value())
        {
            LogError("{}, [Depth Image was not created.]", idxRes.error());
            return false;
        }

        target.depthImg = std::move(*idxRes);
        target.depthImgView =
            util::CreateImageView(mDevice, target.depthImg->img, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

        if (VK_NULL_HANDLE == target.depthImgView)
        {
            return false;
        }
    }

    return true;
}

}// namespace polos::rendering
