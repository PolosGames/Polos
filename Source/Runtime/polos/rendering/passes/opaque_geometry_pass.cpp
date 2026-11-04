///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/opaque_geometry_pass.hpp"

#include "polos/rendering/cache/graphics_pipeline_info.hpp"
#include "polos/rendering/cache/pipeline_cache.hpp"
#include "polos/rendering/cache/shader_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/resources/texture.hpp"
#include "polos/rendering/ssbo/quad_instance.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
#include "polos/rendering/vertex/quad_vertex.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/settings.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_util.hpp"
#include "polos/utils/assert.hpp"

#include <vulkan/vulkan.h>

#include <span>

namespace polos::rendering
{

namespace
{

constexpr std::uint32_t const kCameraUboBinding{0U};
constexpr std::uint32_t const kSamplersBinding{1U};
constexpr std::uint32_t const kInstanceSsboBinding{2U};

// 3 (kMaxFramesInFlight) Set0 (UBO + SSBO) + 24 (kMaxFramesInFlight * kMaxBatches) Set1 (Samplers)
static constexpr std::uint32_t const kTotalDescriptorSets =
    OpaqueGeometryPass::kDescriptorSet0Count + OpaqueGeometryPass::kDescriptorSet1Count;

struct RenderPassInfo
{
    VkRenderPassCreateInfo2 info;
    std::uint32_t           subpassCount{0U};
};

static constexpr RenderPassInfo rpInfo = [] -> RenderPassInfo {
    static constexpr std::array<VkAttachmentDescription2, 2U> const attachments = {
        VkAttachmentDescription2{
            .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .pNext          = nullptr,
            .flags          = 0U,
            .format         = VK_FORMAT_R16G16B16A16_SFLOAT,
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        },
        VkAttachmentDescription2{
            .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .pNext          = nullptr,
            .flags          = 0U,
            .format         = VK_FORMAT_D32_SFLOAT,
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };

    static constexpr std::array<VkAttachmentReference2, 2U> const attachmentReferences = {
        VkAttachmentReference2{
            .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .pNext      = nullptr,
            .attachment = 0U,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        },
        VkAttachmentReference2{
            .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .pNext      = nullptr,
            .attachment = 1U,
            .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        },
    };

    static constexpr std::array<VkSubpassDescription2, 1U> const subpassDescriptions = {
        VkSubpassDescription2{
            .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
            .pNext                   = nullptr,
            .flags                   = 0U,
            .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .viewMask                = 0U,//multiew or sth.
            .inputAttachmentCount    = 0U,
            .pInputAttachments       = nullptr,
            .colorAttachmentCount    = 1U,
            .pColorAttachments       = &attachmentReferences.at(0U),
            .pResolveAttachments     = nullptr,
            .pDepthStencilAttachment = &attachmentReferences.at(1U),
            .preserveAttachmentCount = 0U,
            .pPreserveAttachments    = nullptr,
        },
    };

    static constexpr std::array<VkSubpassDependency2, 2U> const subpassDependencies = {
        VkSubpassDependency2{
            .sType         = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .pNext         = nullptr,
            .srcSubpass    = VK_SUBPASS_EXTERNAL,
            .dstSubpass    = 0U,
            .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0U,
            .viewOffset      = 0U,
        },
        VkSubpassDependency2{
            .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .pNext           = nullptr,
            .srcSubpass      = 0U,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask   = VK_ACCESS_TRANSFER_READ_BIT,
            .dependencyFlags = 0U,
            .viewOffset      = 0U,
        },
    };

    static constexpr VkRenderPassCreateInfo2 const info{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = VK_SIZE_CAST(attachments.size()),
        .pAttachments            = attachments.data(),
        .subpassCount            = VK_SIZE_CAST(subpassDescriptions.size()),
        .pSubpasses              = subpassDescriptions.data(),
        .dependencyCount         = VK_SIZE_CAST(subpassDependencies.size()),
        .pDependencies           = subpassDependencies.data(),
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    return RenderPassInfo{.info = info, .subpassCount = subpassDescriptions.size()};
}();

auto CreateRenderPass() -> VkRenderPass
{
    VkRenderPass pass{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateRenderPass2(RenderContext::GetVulkanDevice().logiDevice, &rpInfo.info, nullptr, &pass),
        "Could not create GeometryPass");

    return pass;
}

auto CreateDescriptorSetLayouts() -> std::array<VkDescriptorSetLayout, 2U>
{
    std::array<VkDescriptorSetLayout, 2U> setLayouts{};

    std::array<VkDescriptorSetLayoutBinding, 2U> dslbSet0{
        VkDescriptorSetLayoutBinding{
            .binding            = kCameraUboBinding,
            .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding{
            .binding            = kInstanceSsboBinding,
            .descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount    = 1U,
            .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    VkDescriptorSetLayoutCreateInfo dslciSet0{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0U,
        .bindingCount = VK_SIZE_CAST(dslbSet0.size()),
        .pBindings    = dslbSet0.data(),
    };

    std::array<VkDescriptorSetLayoutBinding, 1U> dslbSet1{
        VkDescriptorSetLayoutBinding{
            .binding            = kSamplersBinding,
            .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount    = Settings::kMaxSamplers,
            .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    VkDescriptorSetLayoutCreateInfo dslciSet1{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0U,
        .bindingCount = VK_SIZE_CAST(dslbSet1.size()),
        .pBindings    = dslbSet1.data(),
    };

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorSetLayout(
                          RenderContext::GetVulkanDevice().logiDevice,
                          &dslciSet0,
                          nullptr,
                          &setLayouts[0]),
        "Failed to create descriptor set layout 0");
    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorSetLayout(
                          RenderContext::GetVulkanDevice().logiDevice,
                          &dslciSet1,
                          nullptr,
                          &setLayouts[1]),
        "Failed to create descriptor set layout 1");

    return setLayouts;
}

auto CreatePipeline(VkRenderPass tPass, std::array<VkDescriptorSetLayout, 2U>& tSetLayouts) -> VulkanPipeline
{
    std::array<Shader const*, 2U> shaders = {
        RenderContext::GetShaderCache().GetShaderModule("shader.quad.vertex"_sid),
        RenderContext::GetShaderCache().GetShaderModule("shader.quad.fragment"_sid),
    };

    std::array<VkPipelineColorBlendAttachmentState const, 1U> colorBlendAttachments{
        VkPipelineColorBlendAttachmentState{
            .blendEnable         = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask      = static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_R_BIT) |
                                   static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_G_BIT) |
                                   static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_B_BIT) |
                                   static_cast<VkColorComponentFlags>(VK_COLOR_COMPONENT_A_BIT),
        },
    };

    std::array<VkDynamicState const, 2U> dynamicStates = {
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_VIEWPORT,
    };

    GraphicsPipelineInfo pInfo{
        .name                  = "pipeline.quad"_sid,
        .shaders               = shaders,
        .vertexInput           = QuadVertex::CreateVertexInputDescription(),
        .frontFace             = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .colorBlendAttachments = colorBlendAttachments,
        .dynamicStates         = dynamicStates,
        .renderPass            = tPass,
        .subpass               = 0U,
        .descriptorSetLayouts  = tSetLayouts,
    };

    return RenderContext::GetPipelineCache().ConstructPipeline(pInfo);
}

auto CreateDescriptorPool() -> VkDescriptorPool
{
    VkDescriptorPool                     pool{VK_NULL_HANDLE};
    std::array<VkDescriptorPoolSize, 3U> poolSize{
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = Settings::kMaxFramesInFlight * Settings::kMaxSamplers * Settings::kMaxBatches,
        },
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
    };

    VkDescriptorPoolCreateInfo const dpci{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .maxSets       = kTotalDescriptorSets,
        .poolSizeCount = VK_SIZE_CAST(poolSize.size()),
        .pPoolSizes    = poolSize.data(),
    };

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorPool(RenderContext::GetVulkanDevice().logiDevice, &dpci, nullptr, &pool),
        "Failed to create descriptor pool for samplers");

    return pool;
}

auto CreateDescriptorSet0(VkDescriptorPool tPool, VkDescriptorSetLayout tLayout)
    -> std::array<VkDescriptorSet, OpaqueGeometryPass::kDescriptorSet0Count>
{
    std::array<VkDescriptorSetLayout, OpaqueGeometryPass::kDescriptorSet0Count> layouts;
    layouts.fill(tLayout);

    VkDescriptorSetAllocateInfo const dsai{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = tPool,
        .descriptorSetCount = VK_SIZE_CAST(OpaqueGeometryPass::kDescriptorSet0Count),
        .pSetLayouts        = layouts.data(),
    };

    std::array<VkDescriptorSet, OpaqueGeometryPass::kDescriptorSet0Count> sets;

    PL_ASSERT(
        VK_SUCCESS == vkAllocateDescriptorSets(RenderContext::GetVulkanDevice().logiDevice, &dsai, sets.data()),
        "Failed to create descriptor sets");

    return sets;
}

auto CreateDescriptorSet1(VkDescriptorPool tPool, VkDescriptorSetLayout tLayout)
    -> std::array<VkDescriptorSet, OpaqueGeometryPass::kDescriptorSet1Count>
{
    std::array<VkDescriptorSetLayout, OpaqueGeometryPass::kDescriptorSet1Count> layouts;
    layouts.fill(tLayout);

    VkDescriptorSetAllocateInfo const dsai{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = tPool,
        .descriptorSetCount = VK_SIZE_CAST(OpaqueGeometryPass::kDescriptorSet1Count),
        .pSetLayouts        = layouts.data(),
    };

    std::array<VkDescriptorSet, OpaqueGeometryPass::kDescriptorSet1Count> sets;

    PL_ASSERT(
        VK_SUCCESS == vkAllocateDescriptorSets(RenderContext::GetVulkanDevice().logiDevice, &dsai, sets.data()),
        "Failed to create descriptor sets");

    return sets;
}

auto CreateCameraUniformBuffers() -> std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight>
{
    return {CreateCameraUniformBuffer(), CreateCameraUniformBuffer(), CreateCameraUniformBuffer()};
}

auto CreateInstanceBuffers() -> std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight>
{
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> buffers;
    for (auto& buffer : buffers)
    {
        buffer = CreateQuadInstanceStorageBuffer(Settings::kInitialQuadCapacity);
    }
    return buffers;
}

auto UpdateDescriptorSets(VkBuffer tCameraBuffer, VkBuffer tInstanceBuffer, VkDescriptorSet tSet) -> void
{
    VkDescriptorBufferInfo dbiCamera{
        .buffer = tCameraBuffer,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    VkDescriptorBufferInfo dbiSsbo{
        .buffer = tInstanceBuffer,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    std::array<VkWriteDescriptorSet, 2U> descriptor_writes{
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = tSet,
            .dstBinding       = kCameraUboBinding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &dbiCamera,
            .pTexelBufferView = nullptr,
        },
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = tSet,
            .dstBinding       = kInstanceSsboBinding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &dbiSsbo,
            .pTexelBufferView = nullptr,
        },
    };

    vkUpdateDescriptorSets(
        RenderContext::GetVulkanDevice().logiDevice,
        VK_SIZE_CAST(descriptor_writes.size()),
        descriptor_writes.data(),
        0U,
        nullptr);
}

auto UpdateSamplers(std::array<utils::string_id, Settings::kMaxSamplers> const& tTextures, VkDescriptorSet tSet) -> void
{
    std::array<VkDescriptorImageInfo, Settings::kMaxSamplers> diiSamplers;
    for (std::size_t i{0U}; i < Settings::kMaxSamplers; ++i)
    {
        Texture const* texture     = TextureCache::GetTexture(tTextures[i]);
        diiSamplers[i].sampler     = texture->sampler;
        diiSamplers[i].imageView   = texture->view;
        diiSamplers[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkWriteDescriptorSet const imageWrite{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = tSet,
        .dstBinding       = kSamplersBinding,
        .dstArrayElement  = 0U,
        .descriptorCount  = Settings::kMaxSamplers,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = diiSamplers.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(RenderContext::GetVulkanDevice().logiDevice, 1U, &imageWrite, 0U, nullptr);
}

}// namespace

OpaqueGeometryPass::OpaqueGeometryPass()
    : mRenderPass(CreateRenderPass()),
      mSetLayouts(CreateDescriptorSetLayouts()),
      mQuadPipeline(CreatePipeline(mRenderPass, mSetLayouts)),
      mDescriptorPool(CreateDescriptorPool()),
      mDescriptorSet0(CreateDescriptorSet0(mDescriptorPool, mSetLayouts[0])),
      mDescriptorSet1(CreateDescriptorSet1(mDescriptorPool, mSetLayouts[1])),
      mCameraUniforms(CreateCameraUniformBuffers()),
      mInstanceBuffers(CreateInstanceBuffers())
{
    for (size_t i = 0; i < Settings::kMaxFramesInFlight; i++)
    {
        UpdateDescriptorSets(mCameraUniforms[i]->buffer, mInstanceBuffers[i]->buffer, mDescriptorSet0[i]);
    }
}

OpaqueGeometryPass::~OpaqueGeometryPass()
{
    VkDevice device = RenderContext::GetVulkanDevice().logiDevice;
    for (auto* framebuffer : mFramebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    // Destroying the pool also frees all descriptor sets allocated from it.
    vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
    for (auto* layout : mSetLayouts)
    {
        vkDestroyDescriptorSetLayout(device, layout, nullptr);
    }
    vkDestroyRenderPass(device, mRenderPass, nullptr);
}

auto OpaqueGeometryPass::Prepare(std::uint32_t const tFrameSlot, std::span<QuadBatch> tQuadBatches) -> void
{
    PL_ASSERT(tQuadBatches.size() <= Settings::kMaxBatches, "Exceeded max batch count!");

    std::size_t instanceCount{0U};
    for (auto const& batch : tQuadBatches)
    {
        instanceCount += batch.quadInstances.size();
    }

    auto&      instanceBuffer = mInstanceBuffers[tFrameSlot];
    auto const instanceBytes  = instanceCount * sizeof(QuadInstance);
    if (instanceBytes > instanceBuffer->size)
    {
        // BeginFrame has waited for this slot's fence, so its buffer and descriptor set are idle.
        instanceBuffer = CreateQuadInstanceStorageBuffer(instanceCount);
        UpdateDescriptorSets(mCameraUniforms[tFrameSlot]->buffer, instanceBuffer->buffer, mDescriptorSet0[tFrameSlot]);
    }

    mFrameInstances.clear();
    mFrameInstances.reserve(instanceCount);
    for (std::size_t b{0U}; b < tQuadBatches.size(); ++b)
    {
        mFrameInstances.append_range(tQuadBatches[b].quadInstances);
        UpdateSamplers(
            tQuadBatches[b].textures,
            mDescriptorSet1[(static_cast<std::size_t>(tFrameSlot) * Settings::kMaxBatches) + b]);
    }

    if (!mFrameInstances.empty())
    {
        auto const uploaded = instanceBuffer->Write(mFrameInstances.data(), instanceBytes);
        PL_ASSERT(uploaded, "Failed to upload frame instances!");
    }
}

auto OpaqueGeometryPass::Record(Inputs const& tInputs) -> void
{
    mCameraUniforms[tInputs.frameSlot]->Write(&tInputs.cameraUniform, sizeof(CameraUniform));

    {
        std::array<VkImageView, 2U> attachments{tInputs.colorView, tInputs.depthView};
        if (mFramebuffers[tInputs.frameSlot] != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(
                RenderContext::GetVulkanDevice().logiDevice,
                mFramebuffers[tInputs.frameSlot],
                nullptr);
        }
        mFramebuffers[tInputs.frameSlot] = util::CreateFramebuffer(
            RenderContext::GetVulkanDevice().logiDevice,
            mRenderPass,
            attachments,
            tInputs.extent);
    }
    VkFramebuffer currentFb = mFramebuffers[tInputs.frameSlot];

    std::array<VkClearValue const, 2U> clearValues = {
        common::kClearValue,
        VkClearValue{.depthStencil = VkClearDepthStencilValue{.depth = 1.0F, .stencil = 0U}},
    };

    VkRenderPassBeginInfo const rpbInfo{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext           = nullptr,
        .renderPass      = mRenderPass,
        .framebuffer     = currentFb,
        .renderArea      = tInputs.renderArea,
        .clearValueCount = VK_SIZE_CAST(clearValues.size()),
        .pClearValues    = clearValues.data(),
    };

    mCameraUniforms[tInputs.frameSlot]->Write(&tInputs.cameraUniform, sizeof(CameraUniform));

    std::array<VkRect2D, 1U>   scissors{tInputs.scissor};
    std::array<VkViewport, 1U> viewports{tInputs.viewport};

    // TODO(sorbatdev): Command buffer into wrappers
    VkCommandBuffer cmdBuf = tInputs.cmdBuf;

    // clang-format off
    vkCmdBeginRenderPass(cmdBuf, &rpbInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetScissor(cmdBuf, 0U, VK_SIZE_CAST(scissors.size()), scissors.data());
        vkCmdSetViewport(cmdBuf, 0U, VK_SIZE_CAST(viewports.size()), viewports.data());
        vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mQuadPipeline.pipeline);

        vkCmdBindDescriptorSets(cmdBuf,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mQuadPipeline.pipelineLayout,
            0U, 1U, &mDescriptorSet0[tInputs.frameSlot],
            0U, nullptr);

        VkDeviceSize const offset{0U};
        vkCmdBindVertexBuffers(cmdBuf, 0U, 1U, &tInputs.quadVertices->buffer, &offset);
        vkCmdBindIndexBuffer(cmdBuf, tInputs.quadIndices->buffer, 0ULL, VK_INDEX_TYPE_UINT16);

        std::uint32_t firstInstance = 0U;
        for (std::size_t b{0U}; b < tInputs.quadBatches.size(); ++b)
        {
            auto const count = VK_SIZE_CAST(tInputs.quadBatches[b].quadInstances.size());
            vkCmdBindDescriptorSets(cmdBuf,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mQuadPipeline.pipelineLayout,
                1U, 1U, &mDescriptorSet1[(static_cast<std::size_t>(tInputs.frameSlot) * Settings::kMaxBatches) + b],
                0U, nullptr);

            vkCmdDrawIndexed(cmdBuf, 6U, VK_SIZE_CAST(count), 0U, 0U, firstInstance);
            firstInstance += count;
        }
    vkCmdEndRenderPass(cmdBuf);
    // clang-format on
}// namespace polos::rendering

}// namespace polos::rendering
