///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/compositor/render_compositor.hpp"

#include "debug_draw.frag.cpp"
#include "debug_draw.vert.cpp"
#include "forward_shading.frag.cpp"
#include "forward_shading.vert.cpp"
#include "main_compute.comp.cpp"
#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/resources/shader.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
#include "polos/rendering/vk/frame_data.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_profiling.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/scene/scene.hpp"
#include "polos/scene/scene_manager.hpp"
#include "polos/utils/assert.hpp"

#include <span>
#include <utility>

namespace polos::rendering
{

auto RenderCompositor::Create(CreationDetails const& /*tDetails*/) -> Result<void>
{
    auto vertexShader        = Shader::Create(shader::forward_shading_vert::kData, ShaderStage::kVertex);
    auto fragmentShader      = Shader::Create(shader::forward_shading_frag::kData, ShaderStage::kFragment);
    auto computeShader       = Shader::Create(shader::main_compute::kData, ShaderStage::kCompute);
    auto debugVertexShader   = Shader::Create(shader::debug_draw_vert::kData, ShaderStage::kVertex);
    auto debugFragmentShader = Shader::Create(shader::debug_draw_frag::kData, ShaderStage::kFragment);
    if (!vertexShader || !fragmentShader || !computeShader || !debugVertexShader || !debugFragmentShader)
    {
        return ErrorType{RenderingErrc::kFailedCreateShaderModule};
    }

    mForwardVertexShader   = std::move(vertexShader);
    mForwardFragmentShader = std::move(fragmentShader);
    mMainComputeShader     = std::move(computeShader);
    mDebugVertexShader     = std::move(debugVertexShader);
    mDebugFragmentShader   = std::move(debugFragmentShader);

    PL_ASSERT(createTargets().has_value(), "Failed to create render compositor");

    std::array<Buffer*, Settings::kMaxFramesInFlight> cameraUniforms{};
    for (std::size_t i{0U}; i < mCameraUniforms.size(); ++i)
    {
        mCameraUniforms[i] = CreateCameraUniformBuffer();
        cameraUniforms[i]  = mCameraUniforms[i].get();
    }

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

    for (auto& buffer : mLightInstances)
    {
        buffer = Buffer::Create(
            BufferDescription{
                .size               = ecs::Settings::kMaxLights * sizeof(LightInstance),
                .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                .residence          = MemoryResidence::kDevice,
                .hostAccess         = HostAccessFlags::kSeqWrite,
                .persistentlyMapped = true,
                .resizable          = false,
            });
        PL_ASSERT(nullptr != buffer, "Light instance buffer could not be created");
    }

    mForwardShadingPass = std::make_unique<ForwardShadingPass>(
        mForwardVertexShader, mForwardFragmentShader, mMainComputeShader, cameraUniforms);
    mForwardShadingPass->CreateFramebuffers(mFrameTargets, VulkanSwapchain::Get().GetExtent());

    mDebugRenderPass = std::make_unique<DebugRenderPass>(mDebugVertexShader, mDebugFragmentShader, cameraUniforms);
    mDebugRenderPass->CreateFramebuffers(mFrameTargets, VulkanSwapchain::Get().GetExtent());

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
    mDebugRenderPass.reset();
    mForwardShadingPass.reset();
    for (auto& buffer : mCameraUniforms)
    {
        buffer.reset();
    }
    mInstanceBuffer.reset();
    for (auto& buffer : mLightInstances)
    {
        buffer.reset();
    }
    mMainComputeShader.reset();
    mDebugFragmentShader.reset();
    mDebugVertexShader.reset();
    mForwardFragmentShader.reset();
    mForwardVertexShader.reset();
    releaseTargets();
}

auto RenderCompositor::RecreateTargets() -> Result<void>
{
    mUiRenderPass->ReleaseFramebuffers();
    mDebugRenderPass->ReleaseFramebuffers();
    mForwardShadingPass.reset();
    releaseTargets();
    PL_ASSERT(createTargets().has_value(), "Failed to recreate render compositor");

    std::array<Buffer*, Settings::kMaxFramesInFlight> cameraUniforms{};
    for (std::size_t i{0U}; i < mCameraUniforms.size(); ++i)
    {
        cameraUniforms[i] = mCameraUniforms[i].get();
    }

    mForwardShadingPass = std::make_unique<ForwardShadingPass>(
        mForwardVertexShader, mForwardFragmentShader, mMainComputeShader, cameraUniforms);
    mForwardShadingPass->CreateFramebuffers(mFrameTargets, VulkanSwapchain::Get().GetExtent());
    mDebugRenderPass->CreateFramebuffers(mFrameTargets, VulkanSwapchain::Get().GetExtent());
    return {};
}

auto RenderCompositor::Prepare(FrameData const& tFrameData) -> void
{
    POLOS_PROFILE_ZONE("Prepare render compositor");
    scene::Scene* scene = scene::SceneManager::Get().GetCurrentScene();

    // Always at least 1 camera, no need to check if activeCamera exists
    CameraUniform const cameraUbo = GetCameraUniform(
        scene->GetComponent<ecs::CameraComponent>(scene->activeCamera),
        scene->GetComponent<ecs::TransformComponent>(scene->activeCamera), tFrameData.scImage->extent.width,
        tFrameData.scImage->extent.height);
    mCameraUniforms[tFrameData.frameSlot]->Write(&cameraUbo, sizeof(cameraUbo));

    {
        POLOS_PROFILE_ZONE("Upload changed mesh instances");
        auto& renderData = scene->GetRenderData();
        auto& changes    = renderData.pendingMeshUploads;
        POLOS_PROFILE_PLOT("Changed mesh instances", static_cast<std::int64_t>(changes.size()));

        for (auto const index : changes)
        {
            mInstanceBuffer->Write(
                &renderData.meshInstances[index], sizeof(MeshInstance), index * sizeof(MeshInstance));
        }

        changes.clear();
    }

    auto const& renderData  = scene->GetRenderData();
    auto&       lightBuffer = *mLightInstances[tFrameData.frameSlot];
    if (renderData.lightInfo.lightCount != 0U)
    {
        lightBuffer.Write(renderData.lightInstances.data(), renderData.lightInfo.lightCount * sizeof(LightInstance));
    }

    mForwardShadingPass->Prepare(tFrameData.frameSlot, mInstanceBuffer->buffer, lightBuffer.buffer);
}

auto RenderCompositor::Record(FrameData& tFrameData) -> int32_t
{
    POLOS_PROFILE_ZONE("Record render compositor");
    POLOS_PROFILE_GPU_ZONE(tFrameData.currentCmdBuf.GetHandle(), "GPU frame");
    FrameTargets const& targets = mFrameTargets[tFrameData.frameSlot];

    // RenderUpdate runs after Prepare(), so collect debug submissions here.
    mDebugRenderPass->Prepare(tFrameData.frameSlot, RenderingApi::Get().GetDebugDrawQueue());

    scene::Scene* scene = scene::SceneManager::Get().GetCurrentScene();

    int32_t drawCalls{0};
    drawCalls += mForwardShadingPass->Record(
        ForwardShadingPass::Inputs{
            .frameSlot  = tFrameData.frameSlot,
            .cmdBuf     = tFrameData.currentCmdBuf,
            .renderArea = VulkanSwapchain::Get().GetRenderArea(),
            .extent     = ToExtent2D(tFrameData.scImage->extent),
            .scissor    = VulkanSwapchain::Get().GetScissor(),
            .viewport   = VulkanSwapchain::Get().GetViewport(),
            .slotCount  = scene->GetEntityPoolSize(),
            .lightInfo  = scene->GetRenderData().lightInfo,
        });

    drawCalls += mDebugRenderPass->Record({
        .frameSlot  = tFrameData.frameSlot,
        .cmdBuf     = tFrameData.currentCmdBuf,
        .renderArea = VulkanSwapchain::Get().GetRenderArea(),
        .scissor    = VulkanSwapchain::Get().GetScissor(),
        .viewport   = VulkanSwapchain::Get().GetViewport(),
    });

    targets.colorImg->SetUse({
        .layout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .stages   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .accesses = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    });

    {
        POLOS_PROFILE_ZONE("Record ImGui");
        POLOS_PROFILE_GPU_ZONE(tFrameData.currentCmdBuf.GetHandle(), "GPU ImGui");
        mUiRenderPass->Record({
            .frameSlot = tFrameData.frameSlot,
            .cmdBuf    = tFrameData.currentCmdBuf,
            .colorView = targets.colorImgView,
            .extent    = ToExtent2D(tFrameData.scImage->extent),
        });
    }

    targets.colorImg->SetUse({
        .layout   = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .stages   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .accesses = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    });

    {
        POLOS_PROFILE_ZONE("Record blit to swapchain");
        POLOS_PROFILE_GPU_ZONE(tFrameData.currentCmdBuf.GetHandle(), "GPU blit to swapchain");
        tFrameData.scImage->ChangeLayout(tFrameData.currentCmdBuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        tFrameData.scImage->BlitFrom(tFrameData.currentCmdBuf, *targets.colorImg);
        tFrameData.scImage->ChangeLayout(tFrameData.currentCmdBuf, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

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
            vkDestroyImageView(VulkanDevice::Get().logiDevice, target.colorImgView, nullptr);
            target.colorImgView = VK_NULL_HANDLE;
        }
        if (target.depthImgView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(VulkanDevice::Get().logiDevice, target.depthImgView, nullptr);
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
        .extent      = VulkanSwapchain::Get().GetExtent3D(),
        .format      = kSceneColorFormat,
        .usage       = usage,
        .mipLevels   = 1U,
        .arrayLayers = 1U,
        .samples     = kSceneSampleCount,
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
            util::CreateImageView(targets.colorImg->img, kSceneColorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

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
                .width  = VulkanSwapchain::Get().GetExtent3D().width,
                .height = VulkanSwapchain::Get().GetExtent3D().height,
                .depth  = 1U,
            },
        .format      = kSceneDepthFormat,
        .usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .mipLevels   = 1U,
        .arrayLayers = 1U,
        .samples     = kSceneSampleCount,
    };

    for (auto& target : mFrameTargets)
    {
        auto idxRes = Image::Create(depthDesc);
        if (!idxRes)
        {
            LogError("Depth image was not created.");
            return false;
        }

        target.depthImg     = std::move(idxRes);
        target.depthImgView = util::CreateImageView(target.depthImg->img, kSceneDepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

        if (VK_NULL_HANDLE == target.depthImgView)
        {
            return false;
        }
    }

    return true;
}

}// namespace polos::rendering
