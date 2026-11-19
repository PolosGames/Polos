///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/debug_render_pass.hpp"

#include "polos/rendering/common.hpp"
#include "polos/rendering/compositor/frame_targets.hpp"
#include "polos/rendering/passes/debug_geometry.hpp"
#include "polos/rendering/vertex/debug_vertex.hpp"
#include "polos/rendering/vk/command_buffer.hpp"
#include "polos/rendering/vk/descriptor_binding.hpp"
#include "polos/rendering/vk/descriptor_pool_builder.hpp"
#include "polos/rendering/vk/graphics_pipeline_builder.hpp"
#include "polos/rendering/vk/render_pass_builder.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_profiling.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace polos::rendering
{
namespace
{

constexpr uint32_t kSet0{0U};
constexpr uint32_t kPrimitiveCount  = static_cast<uint32_t>(kDebugPrimitiveCount);
constexpr uint32_t kDescriptorCount = Settings::kMaxFramesInFlight * kPrimitiveCount;

constexpr DescriptorBinding kBinding_Ubo_Camera{
    .type    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .set     = kSet0,
    .binding = 0U,
};
constexpr DescriptorBinding kBinding_Ssbo_Instances{.set = kSet0, .binding = 1U};

auto DescriptorIndex(uint32_t tFrameSlot, uint32_t tPrimitive) -> uint32_t
{
    return tFrameSlot * kPrimitiveCount + tPrimitive;
}

}// namespace

DebugRenderPass::DebugRenderPass(
    std::shared_ptr<Shader const> tVertexShader,
    std::shared_ptr<Shader const> tFragmentShader,
    std::span<Buffer*>            tCameraUniforms)
{
    PL_ASSERT(tCameraUniforms.size() == Settings::kMaxFramesInFlight, "Expected one camera buffer per frame slot");

    std::array const geometries{
        detail::CreateDebugSphereGeometry(),
        detail::CreateDebugCircleGeometry(),
        detail::CreateDebugCubeGeometry(),
    };
    for (std::size_t i{0U}; i < geometries.size(); ++i)
    {
        auto const& geometry = geometries[i];
        auto&       mesh     = mMeshes[i];
        mesh.vertices        = Buffer::CreateFixedVertexBuffer(std::as_bytes(std::span{geometry.vertices}));
        mesh.indices         = Buffer::CreateFixedIndexBuffer(std::as_bytes(std::span{geometry.indices}));
        mesh.indexCount      = static_cast<uint32_t>(geometry.indices.size());
        PL_ASSERT(mesh.vertices != nullptr && mesh.indices != nullptr, "Failed to create debug wire mesh");
    }

    {
        DescriptorPoolBuilder builder;
        builder.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, kDescriptorCount);
        builder.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, kDescriptorCount);
        builder.AddSet(kDescriptorCount)
            .AddBinding({
                .binding            = kBinding_Ubo_Camera.binding,
                .descriptorType     = kBinding_Ubo_Camera.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            })
            .AddBinding({
                .binding            = kBinding_Ssbo_Instances.binding,
                .descriptorType     = kBinding_Ssbo_Instances.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            });
        mDescPool = builder.Finalize();
    }

    {
        RenderPassBuilder builder;
        builder.AddAttachment({
            .format        = kSceneColorFormat,
            .samples       = kSceneSampleCount,
            .loadOp        = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp       = VK_ATTACHMENT_STORE_OP_STORE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        });
        builder.AddSubpass()
            .AddColorReference({
                .attachment = 0U,
                .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            })
            .AddDependency({
                .srcSubpass    = VK_SUBPASS_EXTERNAL,
                .dstSubpass    = 0U,
                .srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                .dstStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            })
            .AddDependency({
                .srcSubpass    = 0U,
                .dstSubpass    = VK_SUBPASS_EXTERNAL,
                .srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                .dstStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            });
        mRenderPass = builder.Finalize();
        PL_ASSERT(mRenderPass.GetHandle() != VK_NULL_HANDLE, "Failed to create debug render pass");
    }

    {
        GraphicsPipelineBuilder builder;
        builder.SetName("pipeline.debug"_sid);
        builder.AddShader(std::move(tVertexShader));
        builder.AddShader(std::move(tFragmentShader));
        builder.SetVertexInput(DebugVertex::CreateVertexInputDescription());
        builder.SetTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        builder.SetRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        builder.SetMultisampling(kSceneSampleCount);
        builder.SetDepthState(VK_FALSE, VK_FALSE, VK_COMPARE_OP_ALWAYS);
        builder.SetRenderPass(mRenderPass.GetHandle());
        builder.AddColorAttachment({
            .blendEnable         = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                   VK_COLOR_COMPONENT_A_BIT,
        });
        builder.AddDescriptorSetLayout(mDescPool.GetSetLayoutAtIndex(kSet0));
        mDebugPipeline = builder.Finalize();
        PL_ASSERT(mDebugPipeline.pipeline != VK_NULL_HANDLE, "Failed to finalize the debug graphics pipeline");
    }

    for (uint32_t frameSlot{0U}; frameSlot < Settings::kMaxFramesInFlight; ++frameSlot)
    {
        for (uint32_t primitive{0U}; primitive < kPrimitiveCount; ++primitive)
        {
            mDescPool.UpdateBuffers(
                kSet0, DescriptorIndex(frameSlot, primitive),
                {
                    {.binding = kBinding_Ubo_Camera.binding, .buffer = tCameraUniforms[frameSlot]->buffer},
                });
        }
    }
}

DebugRenderPass::~DebugRenderPass()
{
    ReleaseFramebuffers();
    VulkanPipeline::Reset(mDebugPipeline);
}

auto DebugRenderPass::CreateFramebuffers(std::span<FrameTargets const> tTargets, VkExtent2D const& tExtent) -> void
{
    PL_ASSERT(tTargets.size() == mFramebuffers.size(), "Expected one debug framebuffer per frame slot");
    ReleaseFramebuffers();
    for (std::size_t i{0U}; i < tTargets.size(); ++i)
    {
        std::array attachments{tTargets[i].colorImgView};
        mFramebuffers[i] = util::CreateFramebuffer(mRenderPass.GetHandle(), attachments, tExtent);
        PL_ASSERT(mFramebuffers[i] != VK_NULL_HANDLE, "Failed to create debug framebuffer");
    }
}

auto DebugRenderPass::ReleaseFramebuffers() -> void
{
    auto* const device = VulkanDevice::Get().logiDevice;
    for (auto& framebuffer : mFramebuffers)
    {
        vkDestroyFramebuffer(device, std::exchange(framebuffer, VK_NULL_HANDLE), nullptr);
    }
}

auto DebugRenderPass::Prepare(uint32_t tFrameSlot, DebugDrawQueue const& tQueue) -> void
{
    POLOS_PROFILE_ZONE("Upload debug instances");
    for (uint32_t primitive{0U}; primitive < kPrimitiveCount; ++primitive)
    {
        auto const& instances = tQueue.batches[primitive];
        auto&       batch     = mInstanceBatches[tFrameSlot][primitive];
        PL_ASSERT(instances.size() <= std::numeric_limits<uint32_t>::max(), "Too many debug instances");
        batch.count = static_cast<uint32_t>(instances.size());
        if (instances.empty())
        {
            continue;
        }

        VkDeviceSize const requiredBytes = instances.size() * sizeof(DebugInstance);
        if (!batch.buffer || batch.buffer->size < requiredBytes)
        {
            // Only this frame slot's buffer is replaced, after its fence has completed.
            VkDeviceSize const capacity = std::max(requiredBytes, batch.buffer ? batch.buffer->size * 2U : 0U);
            batch.buffer                = Buffer::Create({
                .size               = capacity,
                .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                .residence          = MemoryResidence::kHost,
                .hostAccess         = HostAccessFlags::kSeqWrite,
                .persistentlyMapped = true,
                .resizable          = false,
            });
            PL_ASSERT(batch.buffer != nullptr, "Failed to create debug instance buffer");
            mDescPool.UpdateBuffers(
                kSet0, DescriptorIndex(tFrameSlot, primitive),
                {
                    {.binding = kBinding_Ssbo_Instances.binding, .buffer = batch.buffer->buffer},
                });
        }
        PL_ASSERT(batch.buffer->Write(instances.data(), requiredBytes), "Failed to upload debug instances");
    }
}

auto DebugRenderPass::Record(Inputs const& tInputs) -> int32_t
{
    auto const& batches = mInstanceBatches[tInputs.frameSlot];
    if (std::ranges::none_of(batches, [](auto const& tBatch) {
            return tBatch.count != 0U;
        }))
    {
        return 0;
    }

    POLOS_PROFILE_ZONE("Record debug wireframes");
    auto& cmdBuf = tInputs.cmdBuf;
    POLOS_PROFILE_GPU_ZONE(cmdBuf.GetHandle(), "GPU debug wireframes");
    cmdBuf.BeginRenderPass(mRenderPass, mFramebuffers[tInputs.frameSlot], tInputs.renderArea);
    cmdBuf.SetScissor(tInputs.scissor);
    cmdBuf.SetViewport(tInputs.viewport);
    cmdBuf.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, mDebugPipeline.pipeline);

    int32_t drawCalls{0};
    for (uint32_t primitive{0U}; primitive < kPrimitiveCount; ++primitive)
    {
        auto const& batch = batches[primitive];
        if (batch.count == 0U)
        {
            continue;
        }
        auto const&      mesh = mMeshes[primitive];
        std::array const sets{mDescPool.GetSetHandle(kSet0, DescriptorIndex(tInputs.frameSlot, primitive))};
        cmdBuf.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mDebugPipeline.layout, 0U, sets);
        cmdBuf.BindVertexBuffer(mesh.vertices->buffer, 0U);
        cmdBuf.BindIndexBuffer(mesh.indices->buffer, 0U, VK_INDEX_TYPE_UINT16);
        cmdBuf.DrawIndexed(mesh.indexCount, batch.count);
        ++drawCalls;
    }
    cmdBuf.EndRenderPass();
    return drawCalls;
}

}// namespace polos::rendering
