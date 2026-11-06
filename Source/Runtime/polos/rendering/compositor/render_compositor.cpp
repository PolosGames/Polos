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
#include "polos/ecs/view.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
#include "polos/rendering/vertex/quad_vertex.hpp"
#include "polos/rendering/vk/frame_data.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/rendering/vk/vulkan_util.hpp"
#include "polos/utils/assert.hpp"

#include <deque>
#include <limits>
#include <span>
#include <utility>

namespace polos::rendering
{

namespace
{
struct QuadTexturePair
{
    utils::string_id          textureName{"texture.polos_default_white"_sid};
    std::vector<QuadInstance> quadInstances;
};

auto ArrangeQuadBatches(
    std::vector<OpaqueGeometryPass::QuadBatch>& tQuadBatches,
    std::vector<QuadTexturePair>&               tQuadPairs,
    std::deque<std::size_t>&                    tInstanceCounts) -> void
{
    // We can start from 0 because white texture is counted only once.
    std::uint32_t nextTextureIndex{0U};

    OpaqueGeometryPass::QuadBatch* currentBatch{nullptr};

    for (auto& quadPair : tQuadPairs)
    {
        // Next Batch
        if (nextTextureIndex >= Settings::kMaxSamplers || nextTextureIndex == 0)
        {
            currentBatch = &tQuadBatches.emplace_back();
            // We now know exactly how many instances will be in this batch
            currentBatch->quadInstances.reserve(tInstanceCounts.front());
            currentBatch->textures.fill("texture.polos_default_white"_sid);
            tInstanceCounts.pop_front();
            nextTextureIndex = 0U;
        }

        // We now know the texture index inside the descriptor, so we apply
        std::ranges::for_each(
            quadPair.quadInstances,
            [nextTextureIndex](auto& textureIndex) {
                textureIndex = nextTextureIndex;
            },
            &QuadInstance::textureIndex);

        currentBatch->quadInstances.append_range(quadPair.quadInstances);
        currentBatch->textures[nextTextureIndex++] = quadPair.textureName;
    }
}

}// namespace

auto RenderCompositor::Create(CreationDetails const& /*tDetails*/) -> Result<void>
{
    if (auto result = createTargets(); !result)
    {
        return result;
    }
    {
        std::array<QuadVertex, 4U> quadVertices{
            QuadVertex{
                .position = glm::vec3{-0.5F, -0.5F, 0.0F},
                .texCoord = glm::vec2{0.0F, 0.0F},
            },
            QuadVertex{
                .position = glm::vec3{0.5F, -0.5F, 0.0F},
                .texCoord = glm::vec2{1.0F, 0.0F},
            },
            QuadVertex{
                .position = glm::vec3{0.5F, 0.5F, 0.0F},
                .texCoord = glm::vec2{1.0F, 1.0F},
            },
            QuadVertex{
                .position = glm::vec3{-0.5F, 0.5F, 0.0F},
                .texCoord = glm::vec2{0.0F, 1.0F},
            },
        };

        mQuadVertexBuffer = Buffer::Create(
            BufferDescription{
                .size       = sizeof(quadVertices),
                .usage      = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .residence  = MemoryResidence::kDevice,
                .hostAccess = HostAccessFlags::kNone,
            },
            std::as_bytes(std::span{quadVertices}));

        PL_ASSERT(nullptr != mQuadVertexBuffer, "Failed to create and initialize quad vertex buffer");
    }

    {
        std::array<std::uint16_t, 6U> quadIndices{
            0U,
            1U,
            2U,
            2U,
            3U,
            0U,
        };

        mQuadIndexBuffer = Buffer::Create(
            BufferDescription{
                .size       = sizeof(quadIndices),
                .usage      = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                .residence  = MemoryResidence::kDevice,
                .hostAccess = HostAccessFlags::kNone,
            },
            std::as_bytes(std::span{quadIndices}));

        PL_ASSERT(nullptr != mQuadIndexBuffer, "Failed to create and initialize quad index buffer");
    }

    mOpaqueGeometryPass = std::make_unique<OpaqueGeometryPass>();
    mUiRenderPass       = std::make_unique<UiRenderPass>();
    if (!mUiRenderPass->Create(mFrameTargets.front().colorImg->format))
    {
        return ErrorType{RenderingErrc::kFailedCreateGui};
    }

    return {};
}

auto RenderCompositor::Destroy() -> void
{
    mUiRenderPass.reset();
    mQuadVertexBuffer.reset();
    mQuadIndexBuffer.reset();
    mOpaqueGeometryPass.reset();
    releaseTargets();
}

auto RenderCompositor::RecreateTargets() -> Result<void>
{
    mUiRenderPass->ReleaseFramebuffers();
    mOpaqueGeometryPass.reset();
    releaseTargets();
    if (auto result = createTargets(); !result)
    {
        return result;
    }
    mOpaqueGeometryPass = std::make_unique<OpaqueGeometryPass>();
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

    {
        mQuadBatches.clear();

        std::vector<QuadTexturePair> quadPairs;

        std::deque<std::size_t> quadInstanceCountsInBatch;

        QuadTexturePair* quadPair{nullptr};
        // Assign quads with their textures
        for (auto const [transform, quad] : ecs::View<ecs::TransformComponent, ecs::QuadRendererComponent>(scene))
        {
            std::size_t quadPairIndex{0U};
            while (quadPairIndex < quadPairs.size())
            {
                if (quad->textureName == quadPairs[quadPairIndex].textureName)
                {
                    break;
                }
                ++quadPairIndex;
            }

            if (quadPairIndex == quadPairs.size())
            {
                if (TextureCache::GetTexture(quad->textureName) == nullptr)
                {
                    auto const texture = TextureCache::CreateTexture(quad->textureName, {});
                    PL_ASSERT(
                        texture != std::numeric_limits<std::size_t>::max(),
                        "Could not upload quad texture {}.",
                        quad->textureName);
                }

                auto& pair            = quadPairs.emplace_back();
                quadPair              = &pair;
                quadPair->textureName = quad->textureName;
            }
            quadPair = &quadPairs[quadPairIndex];

            auto const batchIndex = quadPairIndex / Settings::kMaxSamplers;

            // Create a new batch
            if (batchIndex == quadInstanceCountsInBatch.size())
            {
                quadInstanceCountsInBatch.emplace_back();
            }

            quadPair->quadInstances.push_back(
                QuadInstance{
                    .model           = glm::translate(glm::mat4{1.0F}, transform->position) *
                                       glm::mat4_cast(glm::quat(glm::radians(transform->rotation))) *
                                       glm::scale(glm::mat4{1.0F}, transform->scale),
                    .color           = quad->color,
                    .texCoordScissor = quad->scissor,
                    .textureIndex    = 0U,// invalid for now
                    .tilingFactor    = quad->tilingFactor,
                });

            ++quadInstanceCountsInBatch[batchIndex];
        }

        // Proceed to batching if we have anything in scene
        if (!quadInstanceCountsInBatch.empty())
        {
            ArrangeQuadBatches(mQuadBatches, quadPairs, quadInstanceCountsInBatch);
        }
    }

    mOpaqueGeometryPass->Prepare(tFrameData.frameSlot, mQuadBatches);
}

auto RenderCompositor::Record(FrameData const& tFrameData) -> void
{
    FrameTargets const& targets = mFrameTargets[tFrameData.frameSlot];
    mOpaqueGeometryPass->Record(
        OpaqueGeometryPass::Inputs{
            .frameSlot     = tFrameData.frameSlot,
            .cmdBuf        = tFrameData.currentCmdBuf,
            .renderArea    = RenderContext::GetSwapchain().GetRenderArea(),
            .extent        = ToExtent2D(tFrameData.scImage->extent),
            .scissor       = RenderContext::GetSwapchain().GetScissor(),
            .viewport      = RenderContext::GetSwapchain().GetViewport(),
            .cameraUniform = mCameraUbo,
            .quadBatches   = mQuadBatches,
            .colorView     = targets.colorImgView,
            .depthView     = targets.depthImgView,
            .quadVertices  = mQuadVertexBuffer.get(),
            .quadIndices   = mQuadIndexBuffer.get(),
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
    usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    ImageDescription colorDesc{
        .extent =
            {
                .width  = RenderContext::GetSwapchain().GetExtent3D().width,
                .height = RenderContext::GetSwapchain().GetExtent3D().height,
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
        auto idxRes = Image::Create(colorDesc);
        if (!idxRes)
        {
            LogError("Color image was not created.");
            return false;
        }
        targets.colorImg     = std::move(idxRes);
        targets.colorImgView = util::CreateImageView(
            RenderContext::GetVulkanDevice().logiDevice,
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

        target.depthImg     = std::move(idxRes);
        target.depthImgView = util::CreateImageView(
            RenderContext::GetVulkanDevice().logiDevice,
            target.depthImg->img,
            VK_FORMAT_D32_SFLOAT,
            VK_IMAGE_ASPECT_DEPTH_BIT);

        if (VK_NULL_HANDLE == target.depthImgView)
        {
            return false;
        }
    }

    return true;
}

}// namespace polos::rendering
