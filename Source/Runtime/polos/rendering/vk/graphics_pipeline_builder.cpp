///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/graphics_pipeline_builder.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/resources/shader.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"

#include <utility>

namespace polos::rendering
{

void GraphicsPipelineBuilder::SetName(utils::string_id const tName)
{
    mName = tName;
}

void GraphicsPipelineBuilder::AddShader(std::shared_ptr<Shader const> tShader)
{
    mShaders.push_back(std::move(tShader));
}

void GraphicsPipelineBuilder::SetVertexInput(VertexInputDescription tInput)
{
    mVertexInput = std::move(tInput);
}

void GraphicsPipelineBuilder::SetTopology(VkPrimitiveTopology const tTopology)
{
    mTopology = tTopology;
}

void GraphicsPipelineBuilder::SetRasterization(VkPolygonMode const tPolygonMode, VkCullModeFlags const tCullMode,
    VkFrontFace const tFrontFace, VkBool32 const tDepthBiasEnable)
{
    mPolygonMode     = tPolygonMode;
    mCullMode        = tCullMode;
    mFrontFace       = tFrontFace;
    mDepthBiasEnable = tDepthBiasEnable;
}

void GraphicsPipelineBuilder::SetMultisampling(VkSampleCountFlagBits const tSamples)
{
    mMultisampling = tSamples;
}

void GraphicsPipelineBuilder::SetDepthState(VkBool32 const tTest, VkBool32 const tWrite, VkCompareOp const tCompareOp)
{
    mDepthTestEnable  = tTest;
    mDepthWriteEnable = tWrite;
    mDepthCompareOp   = tCompareOp;
}

void GraphicsPipelineBuilder::AddColorAttachment(VkPipelineColorBlendAttachmentState const tAttachment)
{
    mColorBlendAttachments.push_back(tAttachment);
}

void GraphicsPipelineBuilder::SetDynamicStates(std::span<VkDynamicState const> const tStates)
{
    mDynamicStates.assign(tStates.begin(), tStates.end());
}

void GraphicsPipelineBuilder::SetViewport(VkViewport const tViewport, VkRect2D const tScissor)
{
    mViewport = tViewport;
    mScissor  = tScissor;
}

void GraphicsPipelineBuilder::SetRenderPass(VkRenderPass tRenderPass, std::uint32_t const tSubpass)
{
    mRenderPass = tRenderPass;
    mSubpass    = tSubpass;
}

void GraphicsPipelineBuilder::AddDescriptorSetLayout(VkDescriptorSetLayout tLayout)
{
    mDescriptorSetLayouts.push_back(tLayout);
}

void GraphicsPipelineBuilder::AddPushConstantRange(VkPushConstantRange const tRange)
{
    mPushConstantRanges.push_back(tRange);
}

auto GraphicsPipelineBuilder::Finalize() const -> VulkanPipeline
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(mShaders.size());

    for (auto const& shader : mShaders)
    {
        if (shader == nullptr)
        {
            LogError("Missing graphics shader for pipeline {}.", mName);
            return {};
        }

        shaderStages.push_back({
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = static_cast<VkShaderStageFlagBits>(shader->stage),
            .module              = shader->module,
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        });
    }

    VkPipelineVertexInputStateCreateInfo vertexInput{
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0U,
        .vertexBindingDescriptionCount   = 0U,
        .pVertexBindingDescriptions      = nullptr,
        .vertexAttributeDescriptionCount = 0U,
        .pVertexAttributeDescriptions    = nullptr,
    };
    if (mVertexInput.has_value())
    {
        vertexInput.vertexBindingDescriptionCount   = ToVkSize(mVertexInput->bindings.size());
        vertexInput.pVertexBindingDescriptions      = mVertexInput->bindings.data();
        vertexInput.vertexAttributeDescriptionCount = ToVkSize(mVertexInput->attributes.size());
        vertexInput.pVertexAttributeDescriptions    = mVertexInput->attributes.data();
    }

    VkPipelineInputAssemblyStateCreateInfo const inputAssembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .topology               = mTopology,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo const viewportState{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .viewportCount = 1U,
        .pViewports    = &mViewport,
        .scissorCount  = 1U,
        .pScissors     = &mScissor,
    };

    VkPipelineRasterizationStateCreateInfo const rasterizer{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = mPolygonMode,
        .cullMode                = mCullMode,
        .frontFace               = mFrontFace,
        .depthBiasEnable         = mDepthBiasEnable,
        .depthBiasConstantFactor = 0.0F,
        .depthBiasClamp          = 0.0F,
        .depthBiasSlopeFactor    = 0.0F,
        .lineWidth               = 1.0F,
    };

    VkPipelineMultisampleStateCreateInfo const multisampling{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .rasterizationSamples  = mMultisampling,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0F,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo const depthStencil{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .depthTestEnable       = mDepthTestEnable,
        .depthWriteEnable      = mDepthWriteEnable,
        .depthCompareOp        = mDepthCompareOp,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0F,
        .maxDepthBounds        = 1.0F,
    };

    VkPipelineColorBlendStateCreateInfo const colorBlend{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_NO_OP,
        .attachmentCount = ToVkSize(mColorBlendAttachments.size()),
        .pAttachments    = mColorBlendAttachments.data(),
        .blendConstants  = {0.0F, 0.0F, 0.0F, 0.0F},
    };

    VkPipelineDynamicStateCreateInfo const dynamicState{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = ToVkSize(mDynamicStates.size()),
        .pDynamicStates    = mDynamicStates.data(),
    };

    VkPipelineLayoutCreateInfo const layoutInfo{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = ToVkSize(mDescriptorSetLayouts.size()),
        .pSetLayouts            = mDescriptorSetLayouts.data(),
        .pushConstantRangeCount = ToVkSize(mPushConstantRanges.size()),
        .pPushConstantRanges    = mPushConstantRanges.data(),
    };

    VkDevice       device = VulkanDevice::Get().logiDevice;
    VulkanPipeline result;

    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &result.layout) != VK_SUCCESS)
    {
        LogError("Failed to create a pipeline layout for pipeline {}.", mName);
        return {};
    }

    VkGraphicsPipelineCreateInfo const pipelineInfo{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0U,
        .stageCount          = ToVkSize(shaderStages.size()),
        .pStages             = shaderStages.data(),
        .pVertexInputState   = &vertexInput,
        .pInputAssemblyState = &inputAssembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depthStencil,
        .pColorBlendState    = &colorBlend,
        .pDynamicState       = &dynamicState,
        .layout              = result.layout,
        .renderPass          = mRenderPass,
        .subpass             = mSubpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = 0U,
    };

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1U, &pipelineInfo, nullptr, &result.pipeline) != VK_SUCCESS)
    {
        LogError("Failed to create graphics pipeline {}.", mName);
        VulkanPipeline::Reset(result);
        return {};
    }

    return result;
}

}
