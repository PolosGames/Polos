//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/cache/pipeline_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/cache/graphics_pipeline_info.hpp"
#include "polos/rendering/vk/common.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"
#include "polos/utils/string_id.hpp"
#include "polos/utils/string_map.hpp"

namespace polos::rendering
{

PipelineCache::PipelineCache()  = default;
PipelineCache::~PipelineCache() = default;

auto PipelineCache::Create(CreationDetails const& /*tDetails*/) -> Result<void>
{
    mDevice = RenderContext::GetVulkanDevice().logiDevice;

    return {};
}

auto PipelineCache::GetPipeline(utils::string_id const tPipelineName) const -> VulkanPipeline
{
    auto const itr = mCache.find(tPipelineName);
    if (itr == mCache.end())
    {
        LogError("Failed to find pipeline in cache: {}. Please create it first.", tPipelineName);
        return {};
    };

    return fromCacheEntry(itr->second);
}

auto PipelineCache::GetVkPipeline(utils::string_id const tPipelineName) const -> VkPipeline
{
    auto const itr = mCache.find(tPipelineName);
    if (itr == mCache.end())
    {
        LogError("Failed to find pipeline in cache: {}. Please create it first.", tPipelineName);
        return {};
    };

    return itr->second.pipeline;
}

auto PipelineCache::GetVkPipelineLayout(utils::string_id const tPipelineName) const -> VkPipelineLayout
{
    auto const itr = mCache.find(tPipelineName);
    if (itr == mCache.end())
    {
        LogError("Failed to find pipeline in cache: {}. Please create it first.", tPipelineName);
        return {};
    };

    return itr->second.pipelineLayout;
}

auto PipelineCache::GetDescriptorLayouts(utils::string_id const tPipelineName) const
    -> std::span<VkDescriptorSetLayout const>
{
    auto const itr = mCache.find(tPipelineName);
    if (itr == mCache.end())
    {
        LogError("Failed to find pipeline in cache: {}. Please create it first.", tPipelineName);
        return {};
    };

    return itr->second.dsLayouts;
}

auto PipelineCache::ConstructPipeline(GraphicsPipelineInfo const& tPipelineInfo) -> VulkanPipeline
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

    VulkanSwapchain& swapchain = RenderContext::GetSwapchain();

    utils::string_id const pipeline_key = tPipelineInfo.name;

    auto const itr = mCache.find(pipeline_key);
    if (itr != mCache.end())
    {
        LogWarn(
            "Trying to create a pipeline that already exists in the cache: |{}|",
            utils::GetStringFromID(tPipelineInfo.name));
        // A recreated pass supplies new handles for the same compatible set layouts.
        // Keep the borrowed metadata current; the pass owns the actual layouts.
        itr->second.dsLayouts.assign(
            tPipelineInfo.descriptorSetLayouts.begin(), tPipelineInfo.descriptorSetLayouts.end());
        return fromCacheEntry(itr->second);
    }

    VkPipelineRenderingCreateInfo const pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &swapchain.GetSurfaceFormat().format,
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
        .pViewports    = &swapchain.GetViewport(),
        .scissorCount  = 1U,
        .pScissors     = &swapchain.GetScissor(),
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

    VkPipelineLayoutCreateInfo const pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = VK_SIZE_CAST(tPipelineInfo.descriptorSetLayouts.size()),
        .pSetLayouts            = tPipelineInfo.descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0U,
        .pPushConstantRanges    = nullptr,
    };

    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    if (VK_SUCCESS != vkCreatePipelineLayout(mDevice, &pipeline_layout_info, nullptr, &pipeline_layout))
    {
        LogError("Failed to create a pipeline layout for pipeline {}.", pipeline_key);
        return {};
    }

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
    if (VK_SUCCESS != vkCreateGraphicsPipelines(mDevice, mCreationCache, 1U, &pipeline_info, nullptr, &pipeline))
    {
        LogError("Failed to create pipeline object {}.", pipeline_key);
        if (VK_NULL_HANDLE != pipeline)
        {
            vkDestroyPipeline(mDevice, pipeline, nullptr);
        }
        vkDestroyPipelineLayout(mDevice, pipeline_layout, nullptr);
        return {};
    }

    auto [itr_inserted, was_inserted] = mCache.insert(
        std::make_pair(
            pipeline_key,
            CacheEntry{
                .pipeline       = pipeline,
                .pipelineLayout = pipeline_layout,
                .dsLayouts =
                    std::vector(tPipelineInfo.descriptorSetLayouts.begin(), tPipelineInfo.descriptorSetLayouts.end()),
            }));

    return fromCacheEntry(itr_inserted->second);
}

auto PipelineCache::Destroy() -> void
{
    LogInfo("Destroying and invalidating pipeline cache...");
    for (auto const& [_, pipeline] : mCache)// NOLINT
    {
        if (VK_NULL_HANDLE != pipeline.pipeline)
        {
            vkDestroyPipeline(mDevice, pipeline.pipeline, nullptr);
        }
        if (VK_NULL_HANDLE != pipeline.pipelineLayout)
        {
            vkDestroyPipelineLayout(mDevice, pipeline.pipelineLayout, nullptr);
        }
    }
    mCache.clear();

    if (mCreationCache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(mDevice, mCreationCache, nullptr);
        mCreationCache = VK_NULL_HANDLE;
    }

    mDevice = VK_NULL_HANDLE;
}

auto PipelineCache::fromCacheEntry(CacheEntry const& tEntry) const -> VulkanPipeline
{
    return VulkanPipeline{
        .pipeline       = tEntry.pipeline,
        .pipelineLayout = tEntry.pipelineLayout,
        .dsLayouts      = tEntry.dsLayouts,
    };
}

}// namespace polos::rendering
