///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/forward_shading_pass.hpp"

#include "polos/ecs/settings.hpp"
#include "polos/rendering/compositor/frame_targets.hpp"
#include "polos/rendering/push_constants/compute_push_constants.hpp"
#include "polos/rendering/vertex/primitive_vertex.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/vk/graphics_pipeline_info.hpp"
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
struct DescriptorBinding
{
    VkDescriptorType type{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER};
    uint32_t         set{0U};
    uint32_t         binding{0U};
    uint32_t         countPerFrame{1U};
};

// Set0
constexpr uint32_t const kSet0{0U};
constexpr size_t const   kSet0BindingCount{1U};

constexpr DescriptorBinding const kBinding_Ubo_Camera{
    .type    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .set     = 0U,
    .binding = 0U,
};

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

constexpr size_t const kTotalBindings{kSet0BindingCount + kSet1BindingCount + kSet2BindingCount};

auto CreateRenderPass()
{
    std::array<VkAttachmentDescription2, 2U> attachDesc{

        VkAttachmentDescription2{
            .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .pNext          = nullptr,
            .flags          = 0U,
            .format         = kSceneColorFormat,
            .samples        = kSceneSampleCount,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
        VkAttachmentDescription2{
            .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .pNext          = nullptr,
            .flags          = 0U,
            .format         = kSceneDepthFormat,
            .samples        = kSceneSampleCount,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };

    std::array<VkAttachmentReference2, 2U> attachRef{
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

    std::array<VkSubpassDescription2, 1U> subpassDesc{
        VkSubpassDescription2{
            .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
            .pNext                   = nullptr,
            .flags                   = 0U,
            .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .viewMask                = 0U,
            .inputAttachmentCount    = 0U,
            .pInputAttachments       = nullptr,
            .colorAttachmentCount    = 1U,
            .pColorAttachments       = &attachRef.at(0),
            .pResolveAttachments     = nullptr,
            .pDepthStencilAttachment = &attachRef.at(1),
            .preserveAttachmentCount = 0U,
            .pPreserveAttachments    = nullptr,
        },
    };

    VkMemoryBarrier2 subpass0In = {
        .sType        = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext        = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };
    VkMemoryBarrier2 subpass0Out = {
        .sType        = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext        = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                        VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    std::array<VkSubpassDependency2, 2U> subpassDeps{
        VkSubpassDependency2{
            .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .pNext           = &subpass0In,
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0U,
            .srcStageMask    = 0U,
            .dstStageMask    = 0U,
            .srcAccessMask   = 0U,
            .dstAccessMask   = 0U,
            .dependencyFlags = 0U,
            .viewOffset      = 0,
        },
        VkSubpassDependency2{
            .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .pNext           = &subpass0Out,
            .srcSubpass      = 0U,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = 0U,
            .dstStageMask    = 0U,
            .srcAccessMask   = 0U,
            .dstAccessMask   = 0U,
            .dependencyFlags = 0U,
            .viewOffset      = 0,
        },
    };

    VkRenderPassCreateInfo2 rpci{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = ToVkSize(attachDesc.size()),
        .pAttachments            = attachDesc.data(),
        .subpassCount            = ToVkSize(subpassDesc.size()),
        .pSubpasses              = subpassDesc.data(),
        .dependencyCount         = ToVkSize(subpassDeps.size()),
        .pDependencies           = subpassDeps.data(),
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    VkRenderPass pass{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateRenderPass2(VulkanDevice::Get().logiDevice, &rpci, nullptr, &pass),
        "Failed to create render pass for ForwardShadingPass");
    return pass;
}

auto CreateCompute(Shader const* tComputeShader, std::span<VkDescriptorSetLayout> tLayouts) -> VulkanPipeline
{
    std::array<VkPushConstantRange const, 1U> const pcRanges{
        VkPushConstantRange{
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset     = 0U,
            .size       = sizeof(ComputePushConstants),
        },
    };

    return util::CreateComputePipeline(tComputeShader, tLayouts, pcRanges);
}

auto CreatePBR(VkRenderPass tPass, std::span<VkDescriptorSetLayout> tSetLayouts, std::span<Shader const*> tShaders)
    -> VulkanPipeline
{

    std::array<VkPipelineColorBlendAttachmentState const, 1U> colorBlendAttachments{
        VkPipelineColorBlendAttachmentState{
            .blendEnable         = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
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
        .name                  = "pipeline.cube"_sid,
        .shaders               = tShaders,
        .vertexInput           = PrimitiveVertex::CreateVertexInputDescription(),
        .frontFace             = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .multisampling         = kSceneSampleCount,
        .colorBlendAttachments = colorBlendAttachments,
        .dynamicStates         = dynamicStates,
        .renderPass            = tPass,
        .subpass               = 0U,
        .descriptorSetLayouts  = tSetLayouts,
        .pushConstantRanges    = {},
    };

    return util::CreateGraphicsPipeline(pInfo);
}

auto UpdateSet0(VkBuffer tCameraUbo, DescriptorSet& tSet0) -> void
{
    VkDescriptorBufferInfo const dbi{
        .buffer = tCameraUbo,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    std::array<VkWriteDescriptorSet, kSet0BindingCount> wdss{
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = VK_NULL_HANDLE,
            .dstBinding       = kBinding_Ubo_Camera.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = kBinding_Ubo_Camera.type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &dbi,
            .pTexelBufferView = nullptr,
        },
    };

    tSet0.Update(wdss);
}

auto UpdateSet1(
    VkBuffer       tMeshInstanceSsbo,
    VkBuffer       tIndirectCommandSsbo,
    VkBuffer       tMeshBoundsSsbo,
    DescriptorSet& tSet1) -> void
{
    VkDescriptorBufferInfo const meshInstanceSsbo{
        .buffer = tMeshInstanceSsbo,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    VkDescriptorBufferInfo const indirectCommandsSsbo{
        .buffer = tIndirectCommandSsbo,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    VkDescriptorBufferInfo const meshBoundsSsbo{
        .buffer = tMeshBoundsSsbo,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    std::array<VkWriteDescriptorSet, kSet1BindingCount> wdss{
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = VK_NULL_HANDLE,
            .dstBinding       = kBinding_Ssbo_MeshInstance.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = kBinding_Ssbo_MeshInstance.type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &meshInstanceSsbo,
            .pTexelBufferView = nullptr,
        },
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = VK_NULL_HANDLE,
            .dstBinding       = kBinding_Ssbo_IndirectCommands.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = kBinding_Ssbo_IndirectCommands.type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &indirectCommandsSsbo,
            .pTexelBufferView = nullptr,
        },
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = VK_NULL_HANDLE,
            .dstBinding       = kBinding_Ssbo_MeshBounds.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = kBinding_Ssbo_MeshBounds.type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &meshBoundsSsbo,
            .pTexelBufferView = nullptr,
        },
    };

    tSet1.Update(wdss);
}

auto UpdateSet2(VkBuffer tVisibleMatrixIndicesSsbo, VkBuffer tWorldMatricesSsbo, DescriptorSet& tSet2) -> void
{
    VkDescriptorBufferInfo const visibleMatrixIndicesSsbo{
        .buffer = tVisibleMatrixIndicesSsbo,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    VkDescriptorBufferInfo const worldMatricesSsbo{
        .buffer = tWorldMatricesSsbo,
        .offset = 0U,
        .range  = VK_WHOLE_SIZE,
    };

    std::array<VkWriteDescriptorSet, kSet2BindingCount> wdss{
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = VK_NULL_HANDLE,
            .dstBinding       = kBinding_Ssbo_VisibleMatrixIndices.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = kBinding_Ssbo_VisibleMatrixIndices.type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &visibleMatrixIndicesSsbo,
            .pTexelBufferView = nullptr,
        },
        VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = VK_NULL_HANDLE,
            .dstBinding       = kBinding_Ssbo_WorldMatrices.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = kBinding_Ssbo_WorldMatrices.type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &worldMatricesSsbo,
            .pTexelBufferView = nullptr,
        },
    };

    tSet2.Update(wdss);
}


auto RecordPreComputeBarriers(
    VkCommandBuffer tCmdBuf,
    VkBuffer        tIndirectBuffer,
    VkBuffer        tInstanceBuffer,
    VkBuffer        tMeshBoundsBuffer,
    VkBuffer        tVisibleIndicesBuffer,
    VkBuffer        tWorldMatricesBuffer) -> void
{
    std::array<VkBufferMemoryBarrier2 const, 5U> const bufferBarriers{
        // Flush CPU indirect command reset (instanceCount = 0, firstInstance) & wait for prior indirect draw reads
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_HOST_BIT | VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
            .srcAccessMask       = VK_ACCESS_2_HOST_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tIndirectBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
        // Ensure CPU MeshInstance writes are visible to compute shader
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_HOST_BIT,
            .srcAccessMask       = VK_ACCESS_2_HOST_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tInstanceBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
        // Ensure CPU MeshBounds writes are visible to compute shader
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_HOST_BIT,
            .srcAccessMask       = VK_ACCESS_2_HOST_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tMeshBoundsBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
        // Prevent single-buffered VisibleMatrixIndices WAR hazard while previous frame's vertex shader is reading
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            .srcAccessMask       = VK_ACCESS_2_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tVisibleIndicesBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
        // Prevent single-buffered WorldMatrices WAR hazard while previous frame's vertex shader is reading
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            .srcAccessMask       = VK_ACCESS_2_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tWorldMatricesBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
    };

    VkDependencyInfo const depInfo{
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0U,
        .memoryBarrierCount       = 0U,
        .pMemoryBarriers          = nullptr,
        .bufferMemoryBarrierCount = ToVkSize(bufferBarriers.size()),
        .pBufferMemoryBarriers    = bufferBarriers.data(),
        .imageMemoryBarrierCount  = 0U,
        .pImageMemoryBarriers     = nullptr,
    };

    vkCmdPipelineBarrier2(tCmdBuf, &depInfo);
}

auto RecordPostComputeBarriers(
    VkCommandBuffer tCmdBuf,
    VkBuffer        tIndirectBuffer,
    VkBuffer        tVisibleIndicesBuffer,
    VkBuffer        tWorldMatricesBuffer) -> void
{
    std::array<VkBufferMemoryBarrier2 const, 3U> const bufferBarriers{
        // Make compute atomicAdd(instanceCount) visible to vkCmdDrawIndexedIndirect
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .srcAccessMask       = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
            .dstAccessMask       = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tIndirectBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
        // Make VisibleMatrixIndices writes visible to forward_shading.vert
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .srcAccessMask       = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tVisibleIndicesBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
        // Make WorldMatrices writes visible to forward_shading.vert
        VkBufferMemoryBarrier2{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .srcAccessMask       = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer              = tWorldMatricesBuffer,
            .offset              = 0U,
            .size                = VK_WHOLE_SIZE,
        },
    };

    VkDependencyInfo const depInfo{
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0U,
        .memoryBarrierCount       = 0U,
        .pMemoryBarriers          = nullptr,
        .bufferMemoryBarrierCount = ToVkSize(bufferBarriers.size()),
        .pBufferMemoryBarriers    = bufferBarriers.data(),
        .imageMemoryBarrierCount  = 0U,
        .pImageMemoryBarriers     = nullptr,
    };

    vkCmdPipelineBarrier2(tCmdBuf, &depInfo);
}

}// namespace

ForwardShadingPass::ForwardShadingPass(
    std::shared_ptr<Shader const> tVertexShader,
    std::shared_ptr<Shader const> tFragmentShader,
    std::shared_ptr<Shader const> tMainCompShader)
    : mVertexShader(std::move(tVertexShader)),
      mFragmentShader(std::move(tFragmentShader)),
      mMainComputeShader(std::move(tMainCompShader)),
      mRenderPass(CreateRenderPass()),
      mCameraUniforms(std::array{CreateCameraUniformBuffer(), CreateCameraUniformBuffer(), CreateCameraUniformBuffer()})
{
    std::array<VkDescriptorPoolSize, kTotalBindings> poolSizes{
        VkDescriptorPoolSize{
            .type            = kBinding_Ubo_Camera.type,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
        VkDescriptorPoolSize{
            .type            = kBinding_Ssbo_MeshInstance.type,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
        VkDescriptorPoolSize{
            .type            = kBinding_Ssbo_IndirectCommands.type,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
        VkDescriptorPoolSize{
            .type            = kBinding_Ssbo_MeshBounds.type,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
        VkDescriptorPoolSize{
            .type            = kBinding_Ssbo_VisibleMatrixIndices.type,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
        VkDescriptorPoolSize{
            .type            = kBinding_Ssbo_WorldMatrices.type,
            .descriptorCount = Settings::kMaxFramesInFlight,
        },
    };
    mDescPool = DescriptorPool(poolSizes, 3U * Settings::kMaxFramesInFlight);

    {
        std::array<VkDescriptorSetLayoutBinding, kSet0BindingCount> layoutBindings{
            VkDescriptorSetLayoutBinding{
                .binding            = kBinding_Ubo_Camera.binding,
                .descriptorType     = kBinding_Ubo_Camera.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            },
        };

        auto set0Layout = DescriptorSetLayout::Create(layoutBindings);

        mDescPool.CreateSet(set0Layout);
    }

    {
        std::array<VkDescriptorSetLayoutBinding, kSet1BindingCount> layoutBindings{
            VkDescriptorSetLayoutBinding{
                .binding            = kBinding_Ssbo_MeshInstance.binding,
                .descriptorType     = kBinding_Ssbo_MeshInstance.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding{
                .binding            = kBinding_Ssbo_IndirectCommands.binding,
                .descriptorType     = kBinding_Ssbo_IndirectCommands.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding{
                .binding            = kBinding_Ssbo_MeshBounds.binding,
                .descriptorType     = kBinding_Ssbo_MeshBounds.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            },
        };

        auto set1Layout = DescriptorSetLayout::Create(layoutBindings);

        mDescPool.CreateSet(set1Layout);
    }

    {
        std::array<VkDescriptorSetLayoutBinding, kSet2BindingCount> layoutBindings{
            VkDescriptorSetLayoutBinding{
                .binding            = kBinding_Ssbo_VisibleMatrixIndices.binding,
                .descriptorType     = kBinding_Ssbo_VisibleMatrixIndices.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding{
                .binding            = kBinding_Ssbo_WorldMatrices.binding,
                .descriptorType     = kBinding_Ssbo_WorldMatrices.type,
                .descriptorCount    = 1U,
                .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            },
        };

        auto set2Layout = DescriptorSetLayout::Create(layoutBindings);

        mDescPool.CreateSet(set2Layout);
    }

    std::array<VkDescriptorSetLayout, 3U> setLayouts{
        mDescPool.GetSetLayoutAtIndex(kSet0),
        mDescPool.GetSetLayoutAtIndex(kSet1),
        mDescPool.GetSetLayoutAtIndex(kSet2),
    };

    std::array shaders{mVertexShader.get(), mFragmentShader.get()};
    mPbrPipeline         = CreatePBR(mRenderPass, setLayouts, shaders);
    mComputeMainPipeline = CreateCompute(mMainComputeShader.get(), setLayouts);

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
        UpdateSet0(mCameraUniforms[i]->buffer, mDescPool.GetSetForFrameSlotAtIndex(kSet0, i));
        UpdateSet2(
            mVisibleMatrixIndicesBuffer->buffer,
            mWorldMatrices->buffer,
            mDescPool.GetSetForFrameSlotAtIndex(kSet2, i));
    }
}

ForwardShadingPass::~ForwardShadingPass()
{
    ReleaseFramebuffers();
    VulkanPipeline::Reset(mPbrPipeline);
    VulkanPipeline::Reset(mComputeMainPipeline);

    VkDevice device = VulkanDevice::Get().logiDevice;
    vkDestroyRenderPass(device, mRenderPass, nullptr);
}

auto ForwardShadingPass::CreateFramebuffers(std::span<FrameTargets const> tTargets, VkExtent2D const& tScExtent) -> void
{
    for (std::size_t i{0U}; i < tTargets.size(); ++i)
    {
        std::array attachments{tTargets[i].colorImgView, tTargets[i].depthImgView};

        mFramebuffers[i] = util::CreateFramebuffer(mRenderPass, attachments, tScExtent);
    }
}

auto ForwardShadingPass::ReleaseFramebuffers() -> void
{
    for (auto& framebuffer : mFramebuffers)
    {
        if (VK_NULL_HANDLE != framebuffer)
        {
            vkDestroyFramebuffer(
                VulkanDevice::Get().logiDevice,
                std::exchange(framebuffer, VK_NULL_HANDLE),
                nullptr);
        }
    }
}

auto ForwardShadingPass::Prepare(uint32_t const tFrameSlot, VkBuffer tInstanceBuffer) -> void
{
    if (VK_NULL_HANDLE != tInstanceBuffer)
    {
        UpdateSet1(
            tInstanceBuffer,
            MeshCache::Get().GetIndirectCommandBufferByFrameSlot(tFrameSlot),
            MeshCache::Get().GetMeshBoundsBuffer(),
            mDescPool.GetSetForFrameSlotAtIndex(kSet1, tFrameSlot));
    }
}

auto ForwardShadingPass::Record(Inputs const& tInputs) -> int32_t
{
    POLOS_PROFILE_ZONE("Record forward shading");
    VkFramebuffer const fbuf = mFramebuffers[tInputs.frameSlot];

    std::array<VkDescriptorSet const, 3U> const descriptorSets{
        mDescPool.GetSetForFrameSlotAtIndex(kSet0, tInputs.frameSlot).set,
        mDescPool.GetSetForFrameSlotAtIndex(kSet1, tInputs.frameSlot).set,
        mDescPool.GetSetForFrameSlotAtIndex(kSet2, tInputs.frameSlot).set,
    };

    mCameraUniforms[tInputs.frameSlot]->Write(&tInputs.cameraUbo, sizeof(CameraUniform));

    MeshCache::Get().FlushMeshUses(tInputs.frameSlot);

    VkBuffer const indirectBuffer = MeshCache::Get().GetIndirectCommandBufferByFrameSlot(tInputs.frameSlot);
    uint32_t const meshCount      = MeshCache::Get().GetMeshCount();
    bool const     hasWork        = meshCount > 0U && tInputs.slotCount > 1U && tInputs.instanceBuffer != nullptr;

    VkCommandBuffer const cmdBuf = tInputs.cmdBuf;

    POLOS_PROFILE_PLOT("Compute entity slots", static_cast<std::int64_t>(tInputs.slotCount));
    POLOS_PROFILE_PLOT("Indirect draw commands", static_cast<std::int64_t>(meshCount));
    if (hasWork)
    {
        POLOS_PROFILE_ZONE("Record compute dispatch");
        POLOS_PROFILE_GPU_ZONE(cmdBuf, "GPU instance transform compute");
        RecordPreComputeBarriers(
            cmdBuf,
            indirectBuffer,
            tInputs.instanceBuffer->buffer,
            MeshCache::Get().GetMeshBoundsBuffer(),
            mVisibleMatrixIndicesBuffer->buffer,
            mWorldMatrices->buffer);

        vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, mComputeMainPipeline.pipeline);
        vkCmdBindDescriptorSets(
            cmdBuf,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            mComputeMainPipeline.pipelineLayout,
            0U,
            ToVkSize(descriptorSets.size()),
            descriptorSets.data(),
            0U,
            nullptr);

        ComputePushConstants const pc{.slotCount = tInputs.slotCount};
        vkCmdPushConstants(
            cmdBuf,
            mComputeMainPipeline.pipelineLayout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0U,
            sizeof(ComputePushConstants),
            &pc);

        uint32_t const groupCountX = (tInputs.slotCount + kComputeWorkgroupSize - 1U) / kComputeWorkgroupSize;
        vkCmdDispatch(cmdBuf, groupCountX, 1U, 1U);

        RecordPostComputeBarriers(cmdBuf, indirectBuffer, mVisibleMatrixIndicesBuffer->buffer, mWorldMatrices->buffer);
    }

    std::array<VkClearValue, 2U> const clearValues{
        common::kClearValue,
        VkClearValue{
            .depthStencil = {.depth = 1.0F, .stencil = 0U},
        },
    };

    VkRenderPassBeginInfo const rpbi{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext           = nullptr,
        .renderPass      = mRenderPass,
        .framebuffer     = fbuf,
        .renderArea      = tInputs.renderArea,
        .clearValueCount = ToVkSize(clearValues.size()),
        .pClearValues    = clearValues.data(),
    };

    std::array<VkRect2D, 1U> const   scissors{tInputs.scissor};
    std::array<VkViewport, 1U> const viewports{tInputs.viewport};

    int32_t drawCalls{0};

    POLOS_PROFILE_ZONE("Record indexed indirect draw pass");
    POLOS_PROFILE_GPU_ZONE(cmdBuf, "GPU indexed indirect draw pass");

    // clang-format off
    vkCmdBeginRenderPass(cmdBuf, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetScissor(cmdBuf, 0U, ToVkSize(scissors.size()), scissors.data());
        vkCmdSetViewport(cmdBuf, 0U, ToVkSize(viewports.size()), viewports.data());

        if (hasWork)
        {
            vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPbrPipeline.pipeline);
            vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPbrPipeline.pipelineLayout, 0U, ToVkSize(descriptorSets.size()), descriptorSets.data(), 0U, nullptr);

            VkDeviceSize const offset{0U};
            VkBuffer const     vertBuf = MeshCache::Get().GetVertexBuffer();
            VkBuffer const     indBuf  = MeshCache::Get().GetIndexBuffer();
            vkCmdBindVertexBuffers(cmdBuf, 0U, 1U, &vertBuf, &offset);
            vkCmdBindIndexBuffer(cmdBuf, indBuf, 0ULL, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexedIndirect(cmdBuf, indirectBuffer, offset, meshCount, sizeof(VkDrawIndexedIndirectCommand));
            ++drawCalls;
        }
    vkCmdEndRenderPass(cmdBuf);
    // clang-format on

    return drawCalls;
}
}// namespace polos::rendering
