///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/compositor/render_compositor.hpp"

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/quad_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/scene_manager.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/resources/shader.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
#include "polos/rendering/vertex/quad_vertex.hpp"
#include "polos/rendering/vk/frame_data.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

#include <deque>
#include <limits>
#include <span>
#include <utility>

namespace polos::rendering
{

auto RenderCompositor::Create(CreationDetails const& /*tDetails*/) -> Result<void>
{
    mQuadVertexShader      = Shader::Create("shader.vert.quad"_sid, ShaderStage::kVertex);
    mQuadFragmentShader    = Shader::Create("shader.frag.quad"_sid, ShaderStage::kFragment);
    mForwardVertexShader   = Shader::Create("shader.vert.forward_shading"_sid, ShaderStage::kVertex);
    mForwardFragmentShader = Shader::Create("shader.frag.forward_shading"_sid, ShaderStage::kFragment);
    mMainComputeShader     = Shader::Create("shader.comp.main_compute"_sid, ShaderStage::kCompute);

    PL_ASSERT(createTargets().has_value(), "Failed to create render compositor");

    mInstanceBuffer = Buffer::Create(
        BufferDescription{
            .size               = ecs::Settings::kMaxEntities * sizeof(MeshInstance),
            .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .residence          = MemoryResidence::kDevice,
            .hostAccess         = HostAccessFlags::kSeqWrite,
            .persistentlyMapped = true,
            .resizable          = false,
        });

    PL_ASSERT(nullptr != mInstanceBuffer, "Instance buffer could not be created");

    // mOpaqueGeometryPass = std::make_unique<OpaqueGeometryPass>(mQuadVertexShader, mQuadFragmentShader);
    mForwardShadingPass =
        std::make_unique<ForwardShadingPass>(mForwardVertexShader, mForwardFragmentShader, mMainComputeShader);
    mForwardShadingPass->CreateFramebuffers(mFrameTargets, RenderContext::GetSwapchain().GetExtent());

    mUiRenderPass = std::make_unique<UiRenderPass>();
    if (!mUiRenderPass->Create(mFrameTargets.front().colorImg->format))
    {
        return ErrorType{RenderingErrc::kFailedCreateGui};
    }

    return {};
}

auto RenderCompositor::Destroy() -> void
{
    mUiRenderPass.reset();
    mForwardShadingPass.reset();
    // mOpaqueGeometryPass.reset();
    mInstanceBuffer.reset();
    mMainComputeShader.reset();
    mForwardFragmentShader.reset();
    mForwardVertexShader.reset();
    mQuadFragmentShader.reset();
    mQuadVertexShader.reset();
    releaseTargets();
}

auto RenderCompositor::RecreateTargets() -> Result<void>
{
    mUiRenderPass->ReleaseFramebuffers();
    // mOpaqueGeometryPass.reset();
    mForwardShadingPass.reset();
    releaseTargets();
    PL_ASSERT(createTargets().has_value(), "Failed to recreate render compositor");

    mForwardShadingPass =
        std::make_unique<ForwardShadingPass>(mForwardVertexShader, mForwardFragmentShader, mMainComputeShader);
    mForwardShadingPass->CreateFramebuffers(mFrameTargets, RenderContext::GetSwapchain().GetExtent());
    // mOpaqueGeometryPass = std::make_unique<OpaqueGeometryPass>(mQuadVertexShader, mQuadFragmentShader);
    return {};
}

auto RenderCompositor::Prepare(FrameData const& tFrameData) -> void
{
    ecs::Scene* scene = ecs::SceneManager::GetCurrentScene();

    // Always at least 1 camera, no need to check if activeCamera exists
    mCameraUbo = GetCameraUniform(
        scene->GetComponent<ecs::CameraComponent>(scene->activeCamera),
        scene->GetComponent<ecs::TransformComponent>(scene->activeCamera),
        tFrameData.scImage->extent.width,
        tFrameData.scImage->extent.height);

    auto changes = scene->ConsumeInstanceChanges();

    for (auto const index : changes)
    {
        mInstanceBuffer->Write(&scene->GetMeshInstances()[index], sizeof(MeshInstance), index * sizeof(MeshInstance));
    }

    mForwardShadingPass->Prepare(tFrameData.frameSlot, mInstanceBuffer->buffer);
    // mOpaqueGeometryPass->Prepare(tFrameData.frameSlot, mQuadBatches);
}

auto RenderCompositor::Record(FrameData const& tFrameData) -> int32_t
{
    FrameTargets const& targets = mFrameTargets[tFrameData.frameSlot];
    // mOpaqueGeometryPass->Record(
    //     OpaqueGeometryPass::Inputs{
    //         .frameSlot     = tFrameData.frameSlot,
    //         .cmdBuf        = tFrameData.currentCmdBuf,
    //         .renderArea    = RenderContext::GetSwapchain().GetRenderArea(),
    //         .extent        = ToExtent2D(tFrameData.scImage->extent),
    //         .scissor       = RenderContext::GetSwapchain().GetScissor(),
    //         .viewport      = RenderContext::GetSwapchain().GetViewport(),
    //         .cameraUniform = mCameraUbo,
    //         .colorView     = targets.colorImgView,
    //         .depthView     = targets.depthImgView,
    //         .quadVertices  = mQuadVertexBuffer.get(),
    //         .quadIndices   = mQuadIndexBuffer.get(),
    //     });

    ecs::Scene const* scene = ecs::SceneManager::GetCurrentScene();

    int32_t drawCalls{0};
    drawCalls += mForwardShadingPass->Record(
        ForwardShadingPass::Inputs{
            .frameSlot      = tFrameData.frameSlot,
            .cmdBuf         = tFrameData.currentCmdBuf,
            .renderArea     = RenderContext::GetSwapchain().GetRenderArea(),
            .extent         = ToExtent2D(tFrameData.scImage->extent),
            .scissor        = RenderContext::GetSwapchain().GetScissor(),
            .viewport       = RenderContext::GetSwapchain().GetViewport(),
            .cameraUbo      = mCameraUbo,
            .instanceBuffer = mInstanceBuffer.get(),
            .slotCount      = scene != nullptr ? scene->GetEntityPoolSize() : 0U,
        });

    targets.colorImg->SetUse({
        .layout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .stages   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .accesses = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    });

    mUiRenderPass->Record({
        .frameSlot = tFrameData.frameSlot,
        .cmdBuf    = tFrameData.currentCmdBuf,
        .colorView = targets.colorImgView,
        .extent    = ToExtent2D(tFrameData.scImage->extent),
    });

    targets.colorImg->SetUse({
        .layout   = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .stages   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .accesses = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    });

    tFrameData.scImage->ChangeLayout(tFrameData.currentCmdBuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    tFrameData.scImage->BlitFrom(tFrameData.currentCmdBuf, *targets.colorImg);
    tFrameData.scImage->ChangeLayout(tFrameData.currentCmdBuf, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    return drawCalls;
}

auto RenderCompositor::createTargets() -> Result<void>
{
    if (!createColorTarget())
    {
        releaseTargets();
        return ErrorType{RenderingErrc::kFailedCreateColorTarget};
    }
    if (!createDepthTarget())
    {
        releaseTargets();
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
            vkDestroyImageView(RenderContext::GetVulkanDevice().logiDevice, target.colorImgView, nullptr);
            target.colorImgView = VK_NULL_HANDLE;
        }
        if (target.depthImgView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(RenderContext::GetVulkanDevice().logiDevice, target.depthImgView, nullptr);
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
    usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    ImageDescription colorDesc{
        .extent      = RenderContext::GetSwapchain().GetExtent3D(),
        .format      = VK_FORMAT_R16G16B16A16_SFLOAT,
        .usage       = usage,
        .mipLevels   = 1U,
        .arrayLayers = 1U,
        .samples     = VK_SAMPLE_COUNT_1_BIT,
    };

    for (auto& targets : mFrameTargets)
    {
        auto idxRes = Image::Create(colorDesc);
        if (!idxRes)
        {
            LogError("Color image was not created.");
            return false;
        }
        targets.colorImg = std::move(idxRes);
        targets.colorImgView =
            util::CreateImageView(targets.colorImg->img, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

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
                .width  = RenderContext::GetSwapchain().GetExtent3D().width,
                .height = RenderContext::GetSwapchain().GetExtent3D().height,
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
        auto idxRes = Image::Create(depthDesc);
        if (!idxRes)
        {
            LogError("Depth image was not created.");
            return false;
        }

        target.depthImg = std::move(idxRes);
        target.depthImgView =
            util::CreateImageView(target.depthImg->img, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

        if (VK_NULL_HANDLE == target.depthImgView)
        {
            return false;
        }
    }

    return true;
}

}// namespace polos::rendering
