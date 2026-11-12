///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/common.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/resources/shader.hpp"
#include "polos/rendering/vk/graphics_pipeline_info.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/utils/assert.hpp"

#include <vector>

namespace polos::rendering::util
{

auto CreateGraphicsPipeline(GraphicsPipelineInfo const& tPipelineInfo) -> VulkanPipeline
{
    auto&                               sc = VulkanSwapchain::Get();
    VkPipelineRenderingCreateInfo const pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &sc.GetSurfaceFormat().format,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages(tPipelineInfo.shaders.size());
    for (std::size_t i{0U}; i < tPipelineInfo.shaders.size(); ++i)
    {
        Shader const*                   Shader = tPipelineInfo.shaders[i];
        VkPipelineShaderStageCreateInfo shader_stage_info{
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = static_cast<VkShaderStageFlagBits>(Shader->stage),
            .module              = Shader->module,
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        };
        shader_stages[i] = shader_stage_info;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0U,
        .vertexBindingDescriptionCount   = 0U,
        .pVertexBindingDescriptions      = nullptr,
        .vertexAttributeDescriptionCount = 0U,
        .pVertexAttributeDescriptions    = nullptr,
    };
    if (tPipelineInfo.vertexInput.has_value())
    {
        vertex_input_info.vertexBindingDescriptionCount   = ToVkSize(tPipelineInfo.vertexInput->bindings.size());
        vertex_input_info.pVertexBindingDescriptions      = tPipelineInfo.vertexInput->bindings.data();
        vertex_input_info.vertexAttributeDescriptionCount = ToVkSize(tPipelineInfo.vertexInput->attributes.size());
        vertex_input_info.pVertexAttributeDescriptions    = tPipelineInfo.vertexInput->attributes.data();
    }

    VkPipelineInputAssemblyStateCreateInfo const input_assembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .topology               = tPipelineInfo.topology,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo const viewport_state{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .viewportCount = 1U,
        .pViewports    = &sc.GetViewport(),
        .scissorCount  = 1U,
        .pScissors     = &sc.GetScissor(),
    };

    VkPipelineRasterizationStateCreateInfo const rasterizer{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = tPipelineInfo.polygonMode,
        .cullMode                = tPipelineInfo.cullMode,
        .frontFace               = tPipelineInfo.frontFace,
        .depthBiasEnable         = tPipelineInfo.depthBiasEnable,
        .depthBiasConstantFactor = 0.0F,
        .depthBiasClamp          = 0.0F,
        .depthBiasSlopeFactor    = 0.0F,
        .lineWidth               = 5.0F,
    };

    VkPipelineMultisampleStateCreateInfo const multisampling{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .rasterizationSamples  = tPipelineInfo.multisampling,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0F,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo const depth_stencil{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .depthTestEnable       = tPipelineInfo.depthTestEnable,
        .depthWriteEnable      = tPipelineInfo.depthWriteEnable,
        .depthCompareOp        = tPipelineInfo.depthCompareOp,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0F,
        .maxDepthBounds        = 1.0F,
    };

    VkPipelineColorBlendStateCreateInfo const color_blend{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_NO_OP,
        .attachmentCount = ToVkSize(tPipelineInfo.colorBlendAttachments.size()),
        .pAttachments    = tPipelineInfo.colorBlendAttachments.data(),
        .blendConstants  = {0.0F, 0.0F, 0.0F, 0.0F},
    };

    VkPipelineDynamicStateCreateInfo const dynamic_state{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = ToVkSize(tPipelineInfo.dynamicStates.size()),
        .pDynamicStates    = tPipelineInfo.dynamicStates.data(),
    };

    VkPipelineLayoutCreateInfo const pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = ToVkSize(tPipelineInfo.descriptorSetLayouts.size()),
        .pSetLayouts            = tPipelineInfo.descriptorSetLayouts.data(),
        .pushConstantRangeCount = ToVkSize(tPipelineInfo.pushConstantRanges.size()),
        .pPushConstantRanges    = tPipelineInfo.pushConstantRanges.data(),
    };

    VkPipelineLayout layout{VK_NULL_HANDLE};
    if (VK_SUCCESS !=
        vkCreatePipelineLayout(VulkanDevice::Get().logiDevice, &pipeline_layout_info, nullptr, &layout))
    {
        LogError("Failed to create a pipeline layout for pipeline {}.", tPipelineInfo.name);
        return {};
    }

    VkGraphicsPipelineCreateInfo const pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = tPipelineInfo.renderPass == VK_NULL_HANDLE ? &pipeline_rendering_info : nullptr,
        .flags               = 0U,
        .stageCount          = ToVkSize(shader_stages.size()),
        .pStages             = shader_stages.data(),
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depth_stencil,
        .pColorBlendState    = &color_blend,
        .pDynamicState       = &dynamic_state,
        .layout              = layout,
        .renderPass          = tPipelineInfo.renderPass,
        .subpass             = tPipelineInfo.subpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = 0U,
    };

    VkPipeline pipeline{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateGraphicsPipelines(
                          VulkanDevice::Get().logiDevice,
                          VK_NULL_HANDLE,
                          1U,
                          &pipeline_info,
                          nullptr,
                          &pipeline),
        "Failed to create pipeline object {}.",
        tPipelineInfo.name);

    return VulkanPipeline::Create(pipeline, layout, tPipelineInfo.descriptorSetLayouts);
}

auto CreateComputePipeline(
    Shader const*                        tShader,
    std::span<VkDescriptorSetLayout>     tSetLayouts,
    std::span<VkPushConstantRange const> tPushConstantRanges) -> VulkanPipeline
{
    if (tShader->stage != ShaderStage::kCompute)
    {
        LogError("Shader is not of type compute");
        return {};
    }

    VkSpecializationMapEntry entry{
        .constantID = 0U,
        .offset     = 0U,
        .size       = sizeof(uint32_t),
    };

    VkSpecializationInfo specializationInfo{
        .mapEntryCount = 1,
        .pMapEntries   = &entry,
        .dataSize      = sizeof(uint32_t),
        .pData         = &kComputeWorkgroupSize,
    };

    VkPipelineShaderStageCreateInfo pssci{
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0U,
        .stage               = VK_SHADER_STAGE_COMPUTE_BIT,
        .module              = tShader->module,
        .pName               = "main",
        .pSpecializationInfo = &specializationInfo,
    };

    VkPipelineLayoutCreateInfo plci{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = ToVkSize(tSetLayouts.size()),
        .pSetLayouts            = tSetLayouts.data(),
        .pushConstantRangeCount = ToVkSize(tPushConstantRanges.size()),
        .pPushConstantRanges    = tPushConstantRanges.data(),
    };

    VkPipelineLayout layout{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreatePipelineLayout(VulkanDevice::Get().logiDevice, &plci, nullptr, &layout),
        "Failed to create compute pipeline layout!");

    VkComputePipelineCreateInfo cpci{
        .sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = VK_PIPELINE_CREATE_2_DISPATCH_BASE_BIT,
        .stage              = pssci,
        .layout             = layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = 0U,
    };

    VkPipeline pipeline{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateComputePipelines(
                          VulkanDevice::Get().logiDevice,
                          VK_NULL_HANDLE,
                          1U,
                          &cpci,
                          nullptr,
                          &pipeline),
        "Failed to create compute pipeline");

    return VulkanPipeline::Create(pipeline, layout, tSetLayouts);
}

auto CreateImageView(VkImage tImage, VkFormat tFormat, VkImageAspectFlags tAspectFlags) -> VkImageView
{
    VkImageViewCreateInfo const img_view_info{
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .image    = tImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = tFormat,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange = {
            .aspectMask     = tAspectFlags,
            .baseMipLevel   = 0U,
            .levelCount     = 1U,
            .baseArrayLayer = 0U,
            .layerCount     = 1U,
        },
    };

    VkImageView img_view{VK_NULL_HANDLE};
    if (VK_SUCCESS !=
        vkCreateImageView(VulkanDevice::Get().logiDevice, &img_view_info, nullptr, &img_view))
    {
        LogError("Could not create image view");
        return VK_NULL_HANDLE;
    }

    return img_view;
}

auto CreateFramebuffer(VkRenderPass tRPass, std::span<VkImageView> tAttachments, VkExtent2D const& tExtent)
    -> VkFramebuffer
{
    VkFramebufferCreateInfo const fb_info{
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .renderPass      = tRPass,
        .attachmentCount = ToVkSize(tAttachments.size()),
        .pAttachments    = tAttachments.data(),
        .width           = tExtent.width,
        .height          = tExtent.height,
        .layers          = 1U,
    };

    VkFramebuffer framebuffer{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkCreateFramebuffer(VulkanDevice::Get().logiDevice, &fb_info, nullptr, &framebuffer))
    {
        return VK_NULL_HANDLE;
    }

    return framebuffer;
}

}// namespace polos::rendering::util
