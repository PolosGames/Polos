///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "compositor/render_compositor.hpp"

#include "passes/general_pass.hpp"
#include "polos/logging/log_macros.hpp"
#include "resources/image_description.hpp"
#include "scene/scene_data.hpp"
#include "vk/frame_data.hpp"
#include "vk/render_context.hpp"
#include "vk/vulkan_device.hpp"
#include "vk/vulkan_resource_manager.hpp"
#include "vk/vulkan_swapchain.hpp"
#include "vk/vulkan_util.hpp"

#include <utility>

namespace polos::rendering
{

RenderCompositor::RenderCompositor(RenderContext& tRenderContext)
    : mLastExtent(tRenderContext.GetSwapchain().GetExtent()),
      mGeneralPass(std::make_unique<GeneralPass>(tRenderContext)),
      mDevice(tRenderContext.GetVulkanDevice().mLogiDevice)
{ createTargets(); }

RenderCompositor::~RenderCompositor() = default;

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
        createTargets();
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
        mFrameTargets.colorImg,
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

auto RenderCompositor::createTargets() -> void
{
    {
        auto [img, view] = createColorTarget();

        mFrameTargets.colorImg     = img;
        mFrameTargets.colorImgView = view;
    }

    {
        auto [img, view] = createDepthTarget();

        mFrameTargets.depthImg     = img;
        mFrameTargets.depthImgView = view;
    }
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
    if (mColorImgIdx >= 0)
    {
        VulkanResourceManager::Instance()->DestroyImage(mColorImgIdx);
    }
    if (mDepthImgIdx >= 0)
    {
        VulkanResourceManager::Instance()->DestroyImage(mDepthImgIdx);
    }
    mFrameTargets = {};
    mColorImgIdx  = -1;
    mDepthImgIdx  = -1;
}

auto RenderCompositor::createColorTarget() -> std::pair<VkImage, VkImageView>
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

    auto idxRes = VulkanResourceManager::Instance()->CreateImage(colorDesc);
    if (!idxRes.has_value())
    {
        LogError("{}, [Color Image was not created.]", idxRes.error());
        return {VK_NULL_HANDLE, VK_NULL_HANDLE};
    }

    mColorImgIdx     = *idxRes;
    VkImage     img  = VulkanResourceManager::Instance()->GetImage(*idxRes);
    VkImageView view = util::CreateImageView(mDevice, img, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

    return {img, view};
}

auto RenderCompositor::createDepthTarget() -> std::pair<VkImage, VkImageView>
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

    auto idxRes = VulkanResourceManager::Instance()->CreateImage(depthDesc);
    if (!idxRes.has_value())
    {
        LogError("{}, [Depth Image was not created.]", idxRes.error());
        return {VK_NULL_HANDLE, VK_NULL_HANDLE};
    }

    mDepthImgIdx     = *idxRes;
    VkImage     img  = VulkanResourceManager::Instance()->GetImage(*idxRes);
    VkImageView view = util::CreateImageView(mDevice, img, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

    return {img, view};
}

}// namespace polos::rendering
