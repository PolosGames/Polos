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
    if (tFrameData.scExtent != mLastExtent)
    {
        vkDeviceWaitIdle(mDevice);
        mGeneralPass->Prepare();
        mLastExtent = tFrameData.scExtent;
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
    mGeneralPass->Record(tFrameData, tSceneData, mFrameTargets);

    util::TransitionImageLayout(
        tFrameData.currentCmdBuf,
        tFrameData.scImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT);

    util::CopyImageToImage(
        tFrameData.currentCmdBuf,
        mFrameTargets.colorImg->img,
        tFrameData.scImage,
        util::To3DExtent(mLastExtent),
        util::To3DExtent(tFrameData.scExtent));

    util::TransitionImageLayout(
        tFrameData.currentCmdBuf,
        tFrameData.scImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
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
    if (mFrameTargets.colorImgView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(mDevice, mFrameTargets.colorImgView, nullptr);
    }
    if (mFrameTargets.depthImgView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(mDevice, mFrameTargets.depthImgView, nullptr);
    }
    mFrameTargets.colorImg.reset();
    mFrameTargets.depthImg.reset();
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
        .format = VK_FORMAT_R16G16B16A16_SFLOAT,
        .usage  = usage,
    };

    auto idxRes = GpuImage::Create(colorDesc);
    if (!idxRes.has_value())
    {
        LogError("{}, [Color Image was not created.]", idxRes.error());
        return false;
    }
    mFrameTargets.colorImg     = std::move(*idxRes);
    mFrameTargets.colorImgView = util::CreateImageView(
        mDevice,
        mFrameTargets.colorImg->img,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_ASPECT_COLOR_BIT);

    return VK_NULL_HANDLE != mFrameTargets.colorImgView;
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
        .format = VK_FORMAT_D32_SFLOAT,
        .usage  = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    };

    auto idxRes = GpuImage::Create(depthDesc);
    if (!idxRes.has_value())
    {
        LogError("{}, [Depth Image was not created.]", idxRes.error());
        return false;
    }

    mFrameTargets.depthImg = std::move(*idxRes);
    mFrameTargets.depthImgView =
        util::CreateImageView(mDevice, mFrameTargets.depthImg->img, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

    return VK_NULL_HANDLE != mFrameTargets.depthImgView;
}

}// namespace polos::rendering
