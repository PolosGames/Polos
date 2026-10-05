//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "cache/pipeline_cache.hpp"

#include "cache/graphics_pipeline_info.hpp"
#include "polos/communication/error_code.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "vk/common.hpp"
#include "vk/vulkan_swapchain.hpp"

namespace polos::rendering
{

PipelineCache::PipelineCache()  = default;
PipelineCache::~PipelineCache() = default;

auto PipelineCache::Create(PipelineCacheCreateDetails const& tDetails) -> Result<void>
{
    mDevice    = tDetails.logiDevice;
    mSwapchain = tDetails.swapchain;

    return {};
}

auto PipelineCache::GetPipeline(utils::string_id tPipelineName) const -> Result<VulkanPipeline>
{
    auto const itr = mCache.find(tPipelineName);
    if (itr == mCache.end())
    {
        return ErrorType{RenderingErrc::kFailedFindPipeline};
    };

    return itr->second;
}

auto PipelineCache::GetPipeline(std::string_view const tPipelineName) const -> Result<VulkanPipeline>
{ return GetPipeline(utils::StrHash64(tPipelineName)); }

auto PipelineCache::ConstructPipeline(GraphicsPipelineInfo const& tPipelineInfo) -> Result<VulkanPipeline>
{
    // // --- Pipeline cache gathering stage (OPTIONAL, no errors emitted) ---
    // {// START pipeline cache create
    //     VkPipelineCacheCreateInfo info{
    //         .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
    //         .pNext           = nullptr,
    //         .flags           = 0U,
    //         .initialDataSize = 0U,
    //         .pInitialData    = nullptr,
    //     };

    //     auto res = fs::ReadFile(std::filesystem::path("Resource/Cache/pipeline.dat"));
    //     if (!res.has_value())
    //     {
    //         LogWarn("Something went wrong when opening pipeline cache. |{}|", res.error().Message());
    //     }
    //     else
    //     {
    //         info.initialDataSize = res->uncompressedSize;
    //         info.pInitialData    = res->data.data();
    //     }

    //     if (vkCreatePipelineCache(mDevice, &info, nullptr, &mCreationCache) != VK_SUCCESS)
    //     {
    //         LogWarn("Something went wrong while creating VkPipelineCache.");
    //     }
    // }// END pipeline cache create

    utils::string_id const pipeline_key = tPipelineInfo.name;

    auto const itr = mCache.find(pipeline_key);
    if (itr != mCache.end())
    {
        LogWarn("You are trying to create a pipeline that already exists in the cache: |{}|", tPipelineInfo.name);
        return itr->second;
    }

    VkPipelineRenderingCreateInfo const pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &mSwapchain->GetSurfaceFormat().format,
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
        vertex_input_info.vertexBindingDescriptionCount   = VK_SIZE_CAST(tPipelineInfo.vertexInput->bindings.size());
        vertex_input_info.pVertexBindingDescriptions      = tPipelineInfo.vertexInput->bindings.data();
        vertex_input_info.vertexAttributeDescriptionCount = VK_SIZE_CAST(tPipelineInfo.vertexInput->attributes.size());
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
        .pViewports    = &mSwapchain->GetViewport(),
        .scissorCount  = 1U,
        .pScissors     = &mSwapchain->GetScissor(),
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
        .attachmentCount = VK_SIZE_CAST(tPipelineInfo.colorBlendAttachments.size()),
        .pAttachments    = tPipelineInfo.colorBlendAttachments.data(),
        .blendConstants  = {0.0F, 0.0F, 0.0F, 0.0F},
    };

    VkPipelineDynamicStateCreateInfo const dynamic_state{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = VK_SIZE_CAST(tPipelineInfo.dynamicStates.size()),
        .pDynamicStates    = tPipelineInfo.dynamicStates.data(),
    };

    VkDescriptorSetLayoutCreateInfo const desc_set_layout_info{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0U,
        .bindingCount = VK_SIZE_CAST(tPipelineInfo.descriptorSetLayoutBinding.size()),
        .pBindings    = tPipelineInfo.descriptorSetLayoutBinding.data(),
    };

    VkDescriptorSetLayout desc_set_layout{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreateDescriptorSetLayout(mDevice, &desc_set_layout_info, nullptr, &desc_set_layout));

    std::array<VkDescriptorSetLayout, 1> descriptor_set_layouts{desc_set_layout};

    VkPipelineLayoutCreateInfo const pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = VK_SIZE_CAST(descriptor_set_layouts.size()),
        .pSetLayouts            = descriptor_set_layouts.data(),
        .pushConstantRangeCount = 0U,
        .pPushConstantRanges    = nullptr,
    };

    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreatePipelineLayout(mDevice, &pipeline_layout_info, nullptr, &pipeline_layout));

    VkGraphicsPipelineCreateInfo const pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = tPipelineInfo.renderPass == VK_NULL_HANDLE ? &pipeline_rendering_info : nullptr,
        .flags               = 0U,
        .stageCount          = VK_SIZE_CAST(shader_stages.size()),
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
        .layout              = pipeline_layout,
        .renderPass          = tPipelineInfo.renderPass,
        .subpass             = tPipelineInfo.subpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    VkPipeline pipeline{VK_NULL_HANDLE};
    if (vkCreateGraphicsPipelines(mDevice, mCreationCache, 1U, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kFailedCreatePipeline};
    }

    auto [itr_inserted, was_inserted] = mCache.insert(
        std::make_pair(
            pipeline_key,
            VulkanPipeline{
                .pipeline             = pipeline,
                .layout               = pipeline_layout,
                .descriptorSetLayouts = std::vector(descriptor_set_layouts.begin(), descriptor_set_layouts.end()),
            }));

    return itr_inserted->second;
}

auto PipelineCache::Destroy() -> Result<void>
{
    LogInfo("Destroying and invalidating pipeline cache...");
    for (auto const& [_, pipeline] : mCache)// NOLINT
    {
        if (VK_NULL_HANDLE != pipeline.pipeline)
        {
            vkDestroyPipeline(mDevice, pipeline.pipeline, nullptr);
        }
        if (VK_NULL_HANDLE != pipeline.layout)
        {
            vkDestroyPipelineLayout(mDevice, pipeline.layout, nullptr);
        }
        if (!pipeline.descriptorSetLayouts.empty())
        {
            for (auto const& desc_set_layout : pipeline.descriptorSetLayouts)
            {
                if (VK_NULL_HANDLE != desc_set_layout)
                {
                    vkDestroyDescriptorSetLayout(mDevice, desc_set_layout, nullptr);
                }
            }
        }
    }
    mCache.clear();

    if (mCreationCache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(mDevice, mCreationCache, nullptr);
        mCreationCache = VK_NULL_HANDLE;
    }

    return {};
}

}// namespace polos::rendering
