///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/forward_shading_pass.hpp"

#include "polos/ecs/settings.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/compositor/frame_targets.hpp"
#include "polos/rendering/push_constants/pl_push_constants.hpp"
#include "polos/rendering/push_constants/scene_light_info.hpp"
#include "polos/rendering/ssbo/light_instance.hpp"
#include "polos/rendering/vertex/mesh_vertex.hpp"
#include "polos/rendering/vk/command_buffer.hpp"
#include "polos/rendering/vk/compute_pipeline_builder.hpp"
#include "polos/rendering/vk/descriptor_pool_builder.hpp"
#include "polos/rendering/vk/graphics_pipeline_builder.hpp"
#include "polos/rendering/vk/render_pass_builder.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_profiling.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

#include <array>
#include <cstdint>
#include <span>
#include <utility>

namespace polos::rendering
{

namespace
{
constexpr std::size_t kIndirectBarrierIndex{0U};
constexpr std::size_t kInstanceBarrierIndex{1U};
constexpr std::size_t kMeshBoundsBarrierIndex{2U};

struct DescriptorBinding
{
    VkDescriptorType type{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER};
    uint32_t         set{0U};
    uint32_t         binding{0U};
    uint32_t         countPerFrame{1U};
};

// Set0
constexpr uint32_t const kSet0{0U};

constexpr DescriptorBinding const kBinding_Ubo_Camera{
    .type    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .set     = 0U,
    .binding = 0U,
};
constexpr DescriptorBinding const kBinding_Ssbo_LightInstances{.set = 0U, .binding = 1U};

// Set 1
constexpr uint32_t const kSet1{1U};
constexpr size_t const   kSet1BindingCount{3U};

constexpr DescriptorBinding const kBinding_Ssbo_MeshInstance{.set = 1U, .binding = 0U};
constexpr DescriptorBinding const kBinding_Ssbo_IndirectCommands{.set = 1U, .binding = 1U};
constexpr DescriptorBinding const kBinding_Ssbo_MeshBounds{.set = 1U, .binding = 2U};


// Set 2
constexpr uint32_t const kSet2{2U};
constexpr size_t const   kSet2BindingCount{2U};

constexpr DescriptorBinding const kBinding_Ssbo_VisibleMatrixIndices{.set = 2U, .binding = 0U};
constexpr DescriptorBinding const kBinding_Ssbo_WorldMatrices{.set = 2U, .binding = 1U};


auto CreateRenderPass() -> VulkanRenderPass
{
    constexpr auto stages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
    constexpr auto writes = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    constexpr auto depthRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

    RenderPassBuilder builder;
    builder
        .AddAttachment({
            .format      = kSceneColorFormat,
            .samples     = kSceneSampleCount,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        })
        .AddAttachment({
            .format      = kSceneDepthFormat,
            .samples     = kSceneSampleCount,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        });

    builder.AddSubpass()
        .AddColorReference({
            .attachment = 0U,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        })
        .AddDepthReference({
            .attachment = 1U,
            .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        })
        .AddDependency({
            .srcSubpass    = VK_SUBPASS_EXTERNAL,
            .dstSubpass    = 0U,
            .srcStageMask  = stages,
            .srcAccessMask = writes,
            .dstStageMask  = stages,
            .dstAccessMask = writes | depthRead,
        })
        .AddDependency({
            .srcSubpass    = 0U,
            .dstSubpass    = VK_SUBPASS_EXTERNAL,
            .srcStageMask  = stages,
            .srcAccessMask = writes,
            .dstStageMask  = stages,
            .dstAccessMask = writes | depthRead | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
        });

    auto pass = builder.Finalize();
    PL_ASSERT(pass.GetHandle() != VK_NULL_HANDLE, "Failed to create render pass for ForwardShadingPass");
    return pass;
}

auto BuildDescriptorPool(DescriptorPool& tPool)
{
    DescriptorPoolBuilder builder;
    builder.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Settings::kMaxFramesInFlight);
    builder.AddPoolSize(
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (1U + kSet1BindingCount + kSet2BindingCount) * Settings::kMaxFramesInFlight);

    builder.AddSet(Settings::kMaxFramesInFlight)
        .AddBinding({
            .binding            = kBinding_Ubo_Camera.binding,
            .descriptorType     = kBinding_Ubo_Camera.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        })
        .AddBinding({
            .binding            = kBinding_Ssbo_LightInstances.binding,
            .descriptorType     = kBinding_Ssbo_LightInstances.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        });

    builder.AddSet(Settings::kMaxFramesInFlight)
        .AddBinding({
            .binding            = kBinding_Ssbo_MeshInstance.binding,
            .descriptorType     = kBinding_Ssbo_MeshInstance.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        })
        .AddBinding({
            .binding            = kBinding_Ssbo_IndirectCommands.binding,
            .descriptorType     = kBinding_Ssbo_IndirectCommands.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        })
        .AddBinding({
            .binding            = kBinding_Ssbo_MeshBounds.binding,
            .descriptorType     = kBinding_Ssbo_MeshBounds.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        });

    builder.AddSet(Settings::kMaxFramesInFlight)
        .AddBinding({
            .binding            = kBinding_Ssbo_VisibleMatrixIndices.binding,
            .descriptorType     = kBinding_Ssbo_VisibleMatrixIndices.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        })
        .AddBinding({
            .binding            = kBinding_Ssbo_WorldMatrices.binding,
            .descriptorType     = kBinding_Ssbo_WorldMatrices.type,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        });

    tPool = builder.Finalize();
}

}// namespace

ForwardShadingPass::ForwardShadingPass(
    std::shared_ptr<Shader const> tVertexShader,
    std::shared_ptr<Shader const> tFragmentShader,
    std::shared_ptr<Shader const> tMainCompShader)
    : mRenderPass(CreateRenderPass()),
      mCameraUniforms(std::array{CreateCameraUniformBuffer(), CreateCameraUniformBuffer(), CreateCameraUniformBuffer()})
{
    BuildDescriptorPool(mDescPool);

    std::array<VkDescriptorSetLayout, 4U> graphicsSetLayouts{
        mDescPool.GetSetLayoutAtIndex(kSet0),
        mDescPool.GetSetLayoutAtIndex(kSet1),
        mDescPool.GetSetLayoutAtIndex(kSet2),
        TextureCache::Get().GetBindlessSetLayout(),
    };

    {
        GraphicsPipelineBuilder builder;

        builder.SetName("pipeline.cube"_sid);
        builder.AddShader(std::move(tVertexShader));
        builder.AddShader(std::move(tFragmentShader));

        builder.SetVertexInput(MeshVertex::CreateVertexInputDescription());
        builder.SetRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        builder.SetMultisampling(kSceneSampleCount);

        builder.SetRenderPass(mRenderPass.GetHandle());
        builder.AddColorAttachment({
            .blendEnable         = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                   VK_COLOR_COMPONENT_A_BIT,
        });
        builder.AddPushConstantRange(
            VkPushConstantRange{
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = 0U,
                .size       = sizeof(SceneLightInfo),
            });

        for (auto* const layout : graphicsSetLayouts)
        {
            builder.AddDescriptorSetLayout(layout);
        }

        mPbrPipeline = builder.Finalize();
        PL_ASSERT(mPbrPipeline.pipeline != VK_NULL_HANDLE, "Failed to finalize the forward graphics pipeline");
    }

    {
        ComputePipelineBuilder builder;

        builder.SetShader(std::move(tMainCompShader));
        builder.SetFlags(VK_PIPELINE_CREATE_DISPATCH_BASE_BIT);

        for (auto* const layout : std::span{graphicsSetLayouts}.first<3U>())
        {
            builder.AddDescriptorSetLayout(layout);
        }

        builder.AddPushConstantRange({
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset     = 0U,
            .size       = sizeof(ComputeCullPc),
        });

        constexpr std::array specializationEntries{
            VkSpecializationMapEntry{
                .constantID = 0U,
                .offset     = 0U,
                .size       = sizeof(kComputeWorkgroupSize),
            },
        };
        builder.SetSpecializationConstants(specializationEntries, std::as_bytes(std::span{&kComputeWorkgroupSize, 1U}));

        mComputeMainPipeline = builder.Finalize();
        PL_ASSERT(mComputeMainPipeline.pipeline != VK_NULL_HANDLE, "Failed to finalize the culling compute pipeline");
    }

    mVisibleMatrixIndicesBuffer = Buffer::Create(
        BufferDescription{
            .size               = ecs::Settings::kMaxEntities * sizeof(uint32_t),// ~4 MB
            .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .residence          = MemoryResidence::kDevice,
            .hostAccess         = HostAccessFlags::kNone,
            .persistentlyMapped = false,
            .resizable          = false,
        });
    PL_ASSERT(nullptr != mVisibleMatrixIndicesBuffer, "Failed to create VisibleMatrixIndices buffer");

    mWorldMatrices = Buffer::Create(
        BufferDescription{
            .size               = ecs::Settings::kMaxEntities * sizeof(glm::mat4),// ~64 MB
            .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .residence          = MemoryResidence::kDevice,
            .hostAccess         = HostAccessFlags::kNone,
            .persistentlyMapped = false,
            .resizable          = false,
        });
    PL_ASSERT(nullptr != mWorldMatrices, "Failed to create WorldMatrices buffer");

    for (uint32_t i{0U}; i < Settings::kMaxFramesInFlight; ++i)
    {
        mDescPool.UpdateBuffers(
            kSet0, i,
            {
                {.binding = kBinding_Ubo_Camera.binding, .buffer = mCameraUniforms[i]->buffer},
            });

        mDescPool.UpdateBuffers(
            kSet2, i,
            {
                {.binding = kBinding_Ssbo_VisibleMatrixIndices.binding, .buffer = mVisibleMatrixIndicesBuffer->buffer},
                {.binding = kBinding_Ssbo_WorldMatrices.binding, .buffer = mWorldMatrices->buffer},
            });
    }

    using Scope = DependencyInfoBuilder::Scope;
    constexpr Scope hostWrite{
        .stages = VK_PIPELINE_STAGE_2_HOST_BIT,
        .access = VK_ACCESS_2_HOST_WRITE_BIT,
    };
    constexpr Scope priorVertexRead{
        .stages = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        .access = VK_ACCESS_2_NONE,
    };
    constexpr Scope computeRead{
        .stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .access = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
    };
    constexpr Scope computeWrite{
        .stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .access = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
    };
    constexpr Scope indirectRead{
        .stages = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
        .access = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,
    };
    constexpr Scope vertexRead{
        .stages = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        .access = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
    };

    DependencyInfoBuilder beforeDispatch;
    beforeDispatch
        .AddBufferBarrier(
            VK_NULL_HANDLE, Scope{.stages = hostWrite.stages | indirectRead.stages, .access = hostWrite.access},
            Scope{.stages = computeRead.stages, .access = computeRead.access | computeWrite.access})
        .AddBufferBarrier(VK_NULL_HANDLE, hostWrite, computeRead)
        .AddBufferBarrier(VK_NULL_HANDLE, hostWrite, computeRead)
        .AddBufferBarrier(mVisibleMatrixIndicesBuffer->buffer, priorVertexRead, computeWrite)
        .AddBufferBarrier(mWorldMatrices->buffer, priorVertexRead, computeWrite);

    DependencyInfoBuilder afterDispatch;
    afterDispatch.AddBufferBarrier(VK_NULL_HANDLE, computeWrite, indirectRead)
        .AddBufferBarrier(mVisibleMatrixIndicesBuffer->buffer, computeWrite, vertexRead)
        .AddBufferBarrier(mWorldMatrices->buffer, computeWrite, vertexRead);

    ComputeDependencies const dependencies{
        .beforeDispatch = std::move(beforeDispatch).Finalize(),
        .afterDispatch  = std::move(afterDispatch).Finalize(),
    };
    mComputeDependencies.fill(dependencies);
}

ForwardShadingPass::~ForwardShadingPass()
{
    ReleaseFramebuffers();
    VulkanPipeline::Reset(mPbrPipeline);
    VulkanPipeline::Reset(mComputeMainPipeline);
}

auto ForwardShadingPass::CreateFramebuffers(std::span<FrameTargets const> tTargets, VkExtent2D const& tScExtent) -> void
{
    for (std::size_t i{0U}; i < tTargets.size(); ++i)
    {
        std::array attachments{tTargets[i].colorImgView, tTargets[i].depthImgView};

        mFramebuffers[i] = util::CreateFramebuffer(mRenderPass.GetHandle(), attachments, tScExtent);
    }
}

auto ForwardShadingPass::ReleaseFramebuffers() -> void
{
    auto* const device = VulkanDevice::Get().logiDevice;
    for (auto& framebuffer : mFramebuffers)
    {
        vkDestroyFramebuffer(device, std::exchange(framebuffer, VK_NULL_HANDLE), nullptr);
    }
}

auto ForwardShadingPass::Prepare(uint32_t const tFrameSlot, VkBuffer tInstanceBuffer, VkBuffer tLightBuffer) -> void
{
    auto& meshCache        = MeshCache::Get();
    auto* indirectBuffer   = meshCache.GetIndirectCommandBufferByFrameSlot(tFrameSlot);
    auto* meshBoundsBuffer = meshCache.GetMeshBoundsBuffer();

    mDescPool.UpdateBuffers(
        kSet0, tFrameSlot, {{.binding = kBinding_Ssbo_LightInstances.binding, .buffer = tLightBuffer}});

    mDescPool.UpdateBuffers(
        kSet1, tFrameSlot,
        {
            {
                .binding = kBinding_Ssbo_MeshInstance.binding,
                .buffer  = tInstanceBuffer,
            },
            {
                .binding = kBinding_Ssbo_IndirectCommands.binding,
                .buffer  = indirectBuffer,
            },
            {
                .binding = kBinding_Ssbo_MeshBounds.binding,
                .buffer  = meshBoundsBuffer,
                .range   = meshCache.GetMeshBoundsBufferSize(),
            },
        });

    auto& dependencies = mComputeDependencies[tFrameSlot];
    dependencies.beforeDispatch.UpdateBuffer(kIndirectBarrierIndex, indirectBuffer);
    dependencies.beforeDispatch.UpdateBuffer(kInstanceBarrierIndex, tInstanceBuffer);
    dependencies.beforeDispatch.UpdateBuffer(kMeshBoundsBarrierIndex, meshBoundsBuffer);
    dependencies.afterDispatch.UpdateBuffer(kIndirectBarrierIndex, indirectBuffer);
}

auto ForwardShadingPass::Record(Inputs const& tInputs) -> int32_t
{
    POLOS_PROFILE_ZONE("Record forward shading");
    VkFramebuffer fbuf = mFramebuffers[tInputs.frameSlot];

    std::array<VkDescriptorSet, 4U> const graphicsSets{
        mDescPool.GetSetHandle(kSet0, tInputs.frameSlot),
        mDescPool.GetSetHandle(kSet1, tInputs.frameSlot),
        mDescPool.GetSetHandle(kSet2, tInputs.frameSlot),
        TextureCache::Get().GetBindlessDescriptorSet(),
    };
    auto const computeSets = std::span{graphicsSets}.first<3U>();

    mCameraUniforms[tInputs.frameSlot]->Write(&tInputs.cameraUbo, sizeof(CameraUniform));

    MeshCache::Get().FlushMeshUses(tInputs.frameSlot);

    VkBuffer       indirectBuffer = MeshCache::Get().GetIndirectCommandBufferByFrameSlot(tInputs.frameSlot);
    uint32_t const meshCount      = MeshCache::Get().GetMeshCount();
    bool const     hasWork        = meshCount > 0U && tInputs.slotCount > 1U;

    auto& cmdBuf = tInputs.cmdBuf;

    POLOS_PROFILE_PLOT("Compute entity slots", static_cast<std::int64_t>(tInputs.slotCount));
    POLOS_PROFILE_PLOT("Indirect draw commands", static_cast<std::int64_t>(meshCount));
    if (hasWork)
    {
        POLOS_PROFILE_ZONE("Record compute dispatch");
        POLOS_PROFILE_GPU_ZONE(cmdBuf.GetHandle(), "GPU instance transform compute");
        auto const& dependencies = mComputeDependencies[tInputs.frameSlot];
        cmdBuf.PipelineBarrier2(dependencies.beforeDispatch);

        cmdBuf.BindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, mComputeMainPipeline.pipeline);
        cmdBuf.BindDescriptorSets(VK_PIPELINE_BIND_POINT_COMPUTE, mComputeMainPipeline.layout, 0U, computeSets);

        // NOLINTNEXTLINE
        ComputeCullPc const pc{.data = {.slotCount = tInputs.slotCount}};
        cmdBuf.PushConstants(
            mComputeMainPipeline.layout, VK_SHADER_STAGE_COMPUTE_BIT, 0U, std::as_bytes(std::span{&pc, 1U}));

        uint32_t const groupCountX = (tInputs.slotCount + kComputeWorkgroupSize - 1U) / kComputeWorkgroupSize;
        cmdBuf.Dispatch(groupCountX, 1U, 1U);

        cmdBuf.PipelineBarrier2(dependencies.afterDispatch);
    }

    std::array<VkClearValue, 2U> const clearValues{
        common::kClearValue,
        VkClearValue{
            .depthStencil = {.depth = 1.0F, .stencil = 0U},
        },
    };

    int32_t drawCalls{0};

    POLOS_PROFILE_ZONE("Record indexed indirect draw pass");
    POLOS_PROFILE_GPU_ZONE(cmdBuf.GetHandle(), "GPU indexed indirect draw pass");

    cmdBuf.BeginRenderPass(mRenderPass, fbuf, tInputs.renderArea, clearValues);
    cmdBuf.SetScissor(tInputs.scissor);
    cmdBuf.SetViewport(tInputs.viewport);

    if (hasWork)
    {
        cmdBuf.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, mPbrPipeline.pipeline);

        cmdBuf.PushConstants(
            mPbrPipeline.layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0U, std::as_bytes(std::span{&tInputs.lightInfo, 1U}));
        cmdBuf.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mPbrPipeline.layout, 0U, graphicsSets);

        VkDeviceSize const offset{0U};
        VkBuffer           vertBuf = MeshCache::Get().GetVertexBuffer();
        VkBuffer           indBuf  = MeshCache::Get().GetIndexBuffer();
        cmdBuf.BindVertexBuffer(vertBuf, offset);
        cmdBuf.BindIndexBuffer(indBuf, 0ULL, VK_INDEX_TYPE_UINT32);
        cmdBuf.DrawIndexedIndirect(indirectBuffer, offset, meshCount, sizeof(VkDrawIndexedIndirectCommand));
        ++drawCalls;
    }

    cmdBuf.EndRenderPass();

    return drawCalls;
}
}// namespace polos::rendering
