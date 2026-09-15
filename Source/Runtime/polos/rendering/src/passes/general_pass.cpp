///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "passes/general_pass.hpp"

#include "cache/pipeline_cache.hpp"
#include "cache/shader_cache.hpp"
#include "compositor/frame_targets.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/scene/render_object.hpp"
#include "polos/rendering/scene/scene.hpp"
#include "polos/utils/string_id.hpp"
#include "resources/buffer_description.hpp"
#include "resources/image_description.hpp"
#include "resources/quad_instance.hpp"
#include "resources/uniform_buffer_object.hpp"
#include "scene/camera_uniforms.hpp"
#include "vk/common.hpp"
#include "vk/frame_data.hpp"
#include "vk/render_context.hpp"
#include "vk/render_pass_layout_description.hpp"
#include "vk/vulkan_device.hpp"
#include "vk/vulkan_resource_manager.hpp"
#include "vk/vulkan_swapchain.hpp"
#include "vk/vulkan_util.hpp"

#include <stb_image.h>

namespace polos::rendering
{

static constexpr std::uint64_t kMaxQuadInstances = 10000U;

GeneralPass::GeneralPass(RenderContext& tContext)
    : mContext(tContext),
      mSwapchain(&mContext.GetSwapchain()),
      mVrm(&mContext.GetVulkanResourceManager()),
      mShaderCache(&mContext.GetShaderCache()),
      mPipelineCache(&mContext.GetPipelineCache()),
      mDevice(mContext.GetVulkanDevice().mLogiDevice)
{
    mRenderPass = createRenderPass();

    mPipeline = createPipeline();

    mVertices = {
        Vertex{
            .position = {-0.5F, 0.5F, 0.0F},
            .color    = {1.0F, 0.0F, 0.0F},
            .texCoord = {0.0F, 0.0F},
        },
        Vertex{
            .position = {0.5F, 0.5F, 0.0F},
            .color    = {0.0F, 1.0F, 0.0F},
            .texCoord = {1.0F, 0.0F},
        },
        Vertex{
            .position = {0.5F, -0.5F, 0.0F},
            .color    = {0.0F, 0.0F, 1.0F},
            .texCoord = {1.0F, 1.0F},
        },
        Vertex{
            .position = {-0.5F, -0.5F, 0.0F},
            .color    = {1.0F, 1.0F, 1.0F},
            .texCoord = {0.0F, 1.0F},
        },
    };

    mIndices = {
        2U,
        1U,
        0U,
        0U,
        3U,
        2U,
    };

    mBufferVertices = createVertexBuffer();
    mBufferIndices  = createIndexBuffer();

    mImageViewTuxTexture = createTexture();
    mSamplerTuxTexture   = createTextureSampler();

    std::uint32_t const framesInFlight = mContext.GetFramesInFlight();
    mBufferIndicesUbos.resize(framesInFlight);
    mBufferInstancingIndices.resize(framesInFlight);
    mInstanceMappings.resize(framesInFlight);
    mUboMappings.resize(framesInFlight);
    createUboMapping();

    mDescriptorSets.resize(framesInFlight);
    mDescriptorPool = createDescriptorPool();
    createDescriptorSets();

    mClearVals[0] = VkClearValue{
        .color = {.float32 = {common::kPolosRed, common::kPolosGreen, common::kPolosBlue, 1.0F}},
    };
    mClearVals[1] = VkClearValue{.depthStencil = {.depth = 1.0F, .stencil = 0U}};
}

GeneralPass::~GeneralPass()
{
    for (std::size_t i{0}; i < mBufferIndicesUbos.size(); ++i)
    {
        vmaUnmapMemory(
            mContext.GetVulkanDevice().mAllocator,
            mVrm->GetAllocatedBuffer(mBufferIndicesUbos[i])->allocation);
        mVrm->DestroyBuffer(mBufferIndicesUbos[i]);
    }
    for (std::size_t i{0}; i < mBufferInstancingIndices.size(); ++i)
    {
        vmaUnmapMemory(
            mContext.GetVulkanDevice().mAllocator,
            mVrm->GetAllocatedBuffer(mBufferInstancingIndices[i])->allocation);
        mVrm->DestroyBuffer(mBufferInstancingIndices[i]);
    }
    if (mPassFb != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(mDevice, mPassFb, nullptr);
    }

    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);

    vkDestroySampler(mDevice, mSamplerTuxTexture, nullptr);
    vkDestroyImageView(mDevice, mImageViewTuxTexture, nullptr);
    mVrm->DestroyImage(mTextureImageIndex);

    mVrm->DestroyBuffer(mBufferVerticesIndex);
    mVrm->DestroyBuffer(mBufferIndicesIndex);

    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
}

auto GeneralPass::Prepare() -> void
{
    if (mPassFb != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(mDevice, mPassFb, nullptr);
        mPassFb = VK_NULL_HANDLE;
    }
}

auto GeneralPass::Record(FrameData const& tFrameData, SceneData const& tSceneData, FrameTargets const& tTargets) -> void
{
    mCommandBuffer = tFrameData.currentCmdBuf;

    UniformBufferObject ubo =
        GetCameraUbo(tSceneData.camera, mSwapchain->GetExtent().width, mSwapchain->GetExtent().height);

    std::size_t current_instance_count = std::min(tSceneData.objects.size(), kMaxQuadInstances);

    if (current_instance_count < tSceneData.objects.size())
    {
        // TODO(sorbatdev): Log once
        LogWarn(
            "Number of render objects ({}) exceeds maximum quad instances ({}). Only rendering the first {} objects.",
            tSceneData.objects.size(),
            kMaxQuadInstances,
            current_instance_count);
    }

    //NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    std::vector<QuadInstance> instance_data(current_instance_count);
    for (std::size_t i{0U}; i < current_instance_count; ++i)
    {
        instance_data[i].model = tSceneData.objects[i].transform;
        instance_data[i].color = tSceneData.objects[i].color;
    }

    std::memcpy(
        mInstanceMappings[static_cast<std::size_t>(tFrameData.frameSlot)],
        instance_data.data(),
        sizeof(QuadInstance) * current_instance_count);
    std::memcpy(mUboMappings[static_cast<std::size_t>(tFrameData.frameSlot)], &ubo, sizeof(ubo));

    std::vector<VkImageView> attachments({tTargets.colorImgView, tTargets.depthImgView});
    if (VK_NULL_HANDLE == mPassFb)
    {
        mPassFb = util::CreateFramebuffer(mDevice, mRenderPass, attachments, tFrameData.scExtent);
    }

    VkRenderPassBeginInfo const pass_begin_info{
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = nullptr,
        .renderPass  = mRenderPass,
        .framebuffer = mPassFb,
        .renderArea =
            {
                .offset = VkOffset2D{.x = 0, .y = 0},
                .extent = mSwapchain->GetExtent(),
            },
        .clearValueCount = VK_SIZE_CAST(mClearVals.size()),
        .pClearValues    = mClearVals.data(),
    };

    // clang-format off
        // No transition needed as render pass implicitly transitions from:
        // UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL at the start of the render pass
        // and COLOR_ATTACHMENT_OPTIMAL -> TRANSFER_SRC_OPTIMAL at the end of the render pass

        vkCmdBeginRenderPass(tFrameData.currentCmdBuf, &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(tFrameData.currentCmdBuf, 0U, 1U, &mSwapchain->GetViewport());
            vkCmdSetScissor(tFrameData.currentCmdBuf, 0U, 1U, &mSwapchain->GetScissor());
            vkCmdBindPipeline(tFrameData.currentCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

            // Bind vertex buffer
            VkDeviceSize const offset{0U};
            vkCmdBindVertexBuffers(tFrameData.currentCmdBuf, 0U, 1U, &mBufferVertices, &offset);
            vkCmdBindIndexBuffer(tFrameData.currentCmdBuf, mBufferIndices, offset, VK_INDEX_TYPE_UINT16);
            vkCmdBindDescriptorSets(
                tFrameData.currentCmdBuf,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mPipelineCache->GetPipeline("pl_basiccolor"_sid)->layout,
                0U, 1U, &mDescriptorSets[tFrameData.frameSlot], 0U, nullptr);
            vkCmdDrawIndexed(tFrameData.currentCmdBuf, VK_SIZE_CAST(mIndices.size()), VK_SIZE_CAST(current_instance_count), 0U, 0U, 0U);
        vkCmdEndRenderPass(tFrameData.currentCmdBuf);

    // clang-format on
}

VkRenderPass GeneralPass::createRenderPass()
{
    static constexpr VkAttachmentReference2 const kColorAttachmentRef{
        .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .pNext      = nullptr,
        .attachment = 0U,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };

    static constexpr VkAttachmentReference2 const kDepthAttachmentRef{
        .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .pNext      = nullptr,
        .attachment = 1U,
        .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
    };

    constexpr VkSubpassDescription2 const kSubpassDesc{
        .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .viewMask                = 0U,
        .inputAttachmentCount    = 0U,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1U,
        .pColorAttachments       = &kColorAttachmentRef,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = &kDepthAttachmentRef,
        .preserveAttachmentCount = 0U,
        .pPreserveAttachments    = nullptr,
    };

    constexpr VkSubpassDependency2 const kSubpassDependency{
        .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
        .pNext           = nullptr,
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0U,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0U,
        .viewOffset      = 0U,
    };

    RenderPassLayoutDescription layout{
        .attachments =
            {
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
                    .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                },
            },
        .subpasses    = {kSubpassDesc},
        .dependencies = {kSubpassDependency},
    };

    VkRenderPassCreateInfo2 const pass_info{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = VK_SIZE_CAST(layout.attachments.size()),
        .pAttachments            = layout.attachments.data(),
        .subpassCount            = VK_SIZE_CAST(layout.subpasses.size()),
        .pSubpasses              = layout.subpasses.data(),
        .dependencyCount         = VK_SIZE_CAST(layout.dependencies.size()),
        .pDependencies           = layout.dependencies.data(),
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    VkRenderPass render_pass{VK_NULL_HANDLE};
    assert(vkCreateRenderPass2(mDevice, &pass_info, nullptr, &render_pass) == VK_SUCCESS);

    return render_pass;
}

VkPipeline GeneralPass::createPipeline()
{
    std::array<VkDynamicState, 2> dynamic_states{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    std::array<Shader const*, 2> shaders{
        mShaderCache->GetShaderModule("s_basiccolor_vt"_sid),
        mShaderCache->GetShaderModule("s_basiccolor_fm"_sid),
    };

    std::array<VkPipelineColorBlendAttachmentState, 1> color_blend_attachments{
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

    auto result = mPipelineCache->ConstructPipeline(
        GraphicsPipelineInfo{
            .name        = "pl_basiccolor"_sid,
            .shaders     = shaders,
            .topology    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .vertexInput = CreateVertexDescription(
                VertexAttributes::kWithPosition | VertexAttributes::kWithColors | VertexAttributes::kWithTexCoords),
            .polygonMode                = VK_POLYGON_MODE_FILL,
            .cullMode                   = VK_CULL_MODE_NONE,
            .frontFace                  = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable            = VK_FALSE,
            .multisampling              = VK_SAMPLE_COUNT_1_BIT,
            .depthTestEnable            = VK_TRUE,
            .depthWriteEnable           = VK_TRUE,
            .depthCompareOp             = VK_COMPARE_OP_LESS,
            .colorBlendAttachments      = color_blend_attachments,
            .dynamicStates              = dynamic_states,
            .renderPass                 = mRenderPass,
            .subpass                    = 0U,
            .descriptorSetLayoutBinding = {{
                VkDescriptorSetLayoutBinding{
                    .binding            = 0U,
                    .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount    = 1U,
                    .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
                    .pImmutableSamplers = nullptr,
                },
                VkDescriptorSetLayoutBinding{
                    .binding            = 1U,
                    .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount    = 1U,
                    .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr,
                },
                VkDescriptorSetLayoutBinding{
                    .binding            = 2U,
                    .descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .descriptorCount    = 1U,
                    .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
                    .pImmutableSamplers = nullptr,
                },
            }}
        });

    assert(result.has_value() && "Failed to create basic color pipeline!");

    return result->pipeline;
}

VkBuffer GeneralPass::createVertexBuffer()
{
    VkDeviceSize const buffer_size = sizeof(Vertex) * mVertices.size();

    // TODO(sorbatdev): W6 D27 - stage into kDevice, this is written once and never again
    auto buf = mVrm->CreateBuffer(
        BufferDescription{
            .size       = buffer_size,
            .usage      = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .residence  = MemoryResidence::kHost,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });

    assert(buf.has_value());

    mBufferVerticesIndex = *buf;

    vmaCopyMemoryToAllocation(
        mContext.GetVulkanDevice().mAllocator,
        mVertices.data(),
        mVrm->GetAllocatedBuffer(mBufferVerticesIndex)->allocation,
        0U,
        buffer_size);

    return mVrm->GetBuffer(mBufferVerticesIndex);
}

VkBuffer GeneralPass::createIndexBuffer()
{
    VkDeviceSize const buffer_size = sizeof(std::uint16_t) * mIndices.size();

    // TODO(sorbatdev): W6 D27 - stage into kDevice, this is written once and never again
    auto buf = mVrm->CreateBuffer(
        BufferDescription{
            .size       = buffer_size,
            .usage      = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            .residence  = MemoryResidence::kHost,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });

    assert(buf.has_value());

    mBufferIndicesIndex = *buf;

    vmaCopyMemoryToAllocation(
        mContext.GetVulkanDevice().mAllocator,
        mIndices.data(),
        mVrm->GetAllocatedBuffer(mBufferIndicesIndex)->allocation,
        0U,
        buffer_size);

    return mVrm->GetBuffer(mBufferIndicesIndex);
}

void GeneralPass::createUboMapping()
{
    for (std::size_t i{0U}; i < mContext.GetFramesInFlight(); ++i)
    {
        // UBO
        {
            auto buf = mVrm->CreateBuffer(
                BufferDescription{
                    .size       = sizeof(UniformBufferObject),
                    .usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    .residence  = MemoryResidence::kHost,
                    .hostAccess = HostAccessFlags::kSeqWrite,
                });

            assert(buf.has_value());

            mBufferIndicesUbos[i] = *buf;

            vmaMapMemory(
                mContext.GetVulkanDevice().mAllocator,
                mVrm->GetAllocatedBuffer(mBufferIndicesUbos[i])->allocation,
                &mUboMappings[i]);
        }

        // SSBO
        {
            VkDeviceSize const buffer_size = sizeof(QuadInstance) * kMaxQuadInstances;
            auto               buf         = mVrm->CreateBuffer(
                BufferDescription{
                    .size       = buffer_size,
                    .usage      = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                    .residence  = MemoryResidence::kHost,
                    .hostAccess = HostAccessFlags::kSeqWrite,
                });

            assert(buf.has_value());

            mBufferInstancingIndices[i] = *buf;

            vmaMapMemory(
                mContext.GetVulkanDevice().mAllocator,
                mVrm->GetAllocatedBuffer(mBufferInstancingIndices[i])->allocation,
                &mInstanceMappings[i]);
        }
    }
}

VkDescriptorPool GeneralPass::createDescriptorPool()
{
    std::size_t const                   frames_in_flight = mContext.GetFramesInFlight();
    std::array<VkDescriptorPoolSize, 3> pool_sizes{
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = VK_SIZE_CAST(frames_in_flight),
        },
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = VK_SIZE_CAST(frames_in_flight),
        },
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = VK_SIZE_CAST(frames_in_flight),
        },
    };

    VkDescriptorPoolCreateInfo const pool_info{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .maxSets       = VK_SIZE_CAST(frames_in_flight),
        .poolSizeCount = VK_SIZE_CAST(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data(),
    };

    VkDescriptorPool descriptor_pool{VK_NULL_HANDLE};
    assert(vkCreateDescriptorPool(mDevice, &pool_info, nullptr, &descriptor_pool) == VK_SUCCESS);

    return descriptor_pool;
}

void GeneralPass::createDescriptorSets()
{
    std::size_t const frames_in_flight = mContext.GetFramesInFlight();

    std::vector<VkDescriptorSetLayout> layouts(
        frames_in_flight,
        mPipelineCache->GetPipeline("pl_basiccolor"_sid)->descriptorSetLayouts[0U]);

    VkDescriptorSetAllocateInfo const alloc_info{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = mDescriptorPool,
        .descriptorSetCount = VK_SIZE_CAST(frames_in_flight),
        .pSetLayouts        = layouts.data(),
    };

    assert(vkAllocateDescriptorSets(mDevice, &alloc_info, mDescriptorSets.data()) == VK_SUCCESS);

    for (size_t i = 0; i < frames_in_flight; i++)
    {
        VkDescriptorBufferInfo buffer_info{
            .buffer = mVrm->GetBuffer(mBufferIndicesUbos[i]),
            .offset = 0U,
            .range  = VK_WHOLE_SIZE,
        };

        VkDescriptorBufferInfo ssbo_info{
            .buffer = mVrm->GetBuffer(mBufferInstancingIndices[i]),
            .offset = 0U,
            .range  = VK_WHOLE_SIZE,
        };

        VkDescriptorImageInfo image_info{
            .sampler     = mSamplerTuxTexture,
            .imageView   = mImageViewTuxTexture,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        std::array<VkWriteDescriptorSet, 3> descriptor_writes{
            VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = mDescriptorSets[i],
                .dstBinding       = 0U,
                .dstArrayElement  = 0U,
                .descriptorCount  = 1U,
                .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo       = nullptr,
                .pBufferInfo      = &buffer_info,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = mDescriptorSets[i],
                .dstBinding       = 1U,
                .dstArrayElement  = 0U,
                .descriptorCount  = 1U,
                .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo       = &image_info,
                .pBufferInfo      = nullptr,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = mDescriptorSets[i],
                .dstBinding       = 2U,
                .dstArrayElement  = 0U,
                .descriptorCount  = 1U,
                .descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pImageInfo       = nullptr,
                .pBufferInfo      = &ssbo_info,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets(mDevice, VK_SIZE_CAST(descriptor_writes.size()), descriptor_writes.data(), 0U, nullptr);
    }
}

VkImageView GeneralPass::createTexture()
{
    auto img_res = mVrm->LoadImageResourceToVkImage(
        "Resource/Textures/tux.png",
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    mTextureImageIndex = *img_res;

    return util::CreateImageView(
        mDevice,
        mVrm->GetImage(mTextureImageIndex),
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT);
}

VkSampler GeneralPass::createTextureSampler()
{
    VkPhysicalDeviceProperties phys_dev_props{};
    vkGetPhysicalDeviceProperties(mContext.GetVulkanDevice().mPhysDevice, &phys_dev_props);

    VkSamplerCreateInfo const sampler_info{
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .magFilter               = VK_FILTER_LINEAR,
        .minFilter               = VK_FILTER_LINEAR,
        .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias              = 0.0F,
        .anisotropyEnable        = VK_TRUE,
        .maxAnisotropy           = phys_dev_props.limits.maxSamplerAnisotropy,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 0.0F,
        .maxLod                  = VK_LOD_CLAMP_NONE,
        .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler sampler{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreateSampler(mDevice, &sampler_info, nullptr, &sampler));

    return sampler;
}
}// namespace polos::rendering
