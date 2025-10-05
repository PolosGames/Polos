//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/pipeline_cache.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/graphics_pipeline_info.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"
#include <vulkan/vulkan_core.h>

namespace polos::rendering
{

auto PipelineCache::GetPipeline(graphics_pipeline_info const& t_pipeline_info) -> VkPipeline
{
    auto pipeline_key = std::hash<graphics_pipeline_info>{}(t_pipeline_info);

    auto it = m_cache.find(pipeline_key);
    if (it == m_cache.end())
    {
        LogWarn("Requested pipeline was not found in pipeline cache. Create it with LoadOrCreatePipeline!");
        return VK_NULL_HANDLE;
    };

    return it->second;
}

auto PipelineCache::LoadOrCreatePipeline(graphics_pipeline_info const& t_pipeline_info) -> VkPipeline
{
    // --- Pipeline cache gathering stage (OPTIONAL, no errors emitted) ---
    {// START pipeline cache create
        VkPipelineCacheCreateInfo info{
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0U,
            .initialDataSize = 0U,
            .pInitialData    = nullptr,
        };

        auto res = fs::ReadFile(std::filesystem::path("Resource/Cache/pipeline.dat"));
        if (!res.has_value())
        {
            LogWarn("Something went wrong when opening pipeline cache. |{}|", res.error().Message());
        }
        else
        {
            info.initialDataSize = res->uncompressed_size;
            info.pInitialData    = res->data.data();
        }

        if (vkCreatePipelineCache(m_device, &info, nullptr, &m_creation_cache) != VK_SUCCESS)
        {
            LogWarn("Something went wrong while creating VkPipelineCache.");
        }
    }// END pipeline cache create

    std::size_t pipeline_key = std::hash<graphics_pipeline_info>{}(t_pipeline_info);

    VkPipelineRenderingCreateInfo pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &m_swapchain->GetSurfaceFormat().format,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    VkPipelineShaderStageCreateInfo shader_stages[2]{
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = VK_SHADER_STAGE_VERTEX_BIT,
            .module              = t_pipeline_info.vertex_shader.module,
            .pName               = "vertexMain",
            .pSpecializationInfo = nullptr,
        },
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module              = t_pipeline_info.fragment_shader.module,
            .pName               = "fragmentMain",
            .pSpecializationInfo = nullptr,
        },
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0U,
        .vertexBindingDescriptionCount   = static_cast<std::uint32_t>(t_pipeline_info.vertex_input.bindings.size()),
        .pVertexBindingDescriptions      = t_pipeline_info.vertex_input.bindings.data(),
        .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(t_pipeline_info.vertex_input.attributes.size()),
        .pVertexAttributeDescriptions    = t_pipeline_info.vertex_input.attributes.data(),
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .topology               = t_pipeline_info.topology,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewport_state{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .viewportCount = 1U,
        .pViewports    = &m_swapchain->GetViewport(),
        .scissorCount  = 1U,
        .pScissors     = &m_swapchain->GetScissor(),
    };

    VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = t_pipeline_info.polygon_mode,
        .cullMode                = t_pipeline_info.cull_mode,
        .frontFace               = t_pipeline_info.front_face,
        .depthBiasEnable         = t_pipeline_info.depth_bias_enable,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .rasterizationSamples  = t_pipeline_info.multisampling,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .depthTestEnable       = t_pipeline_info.depth_test_enable,
        .depthWriteEnable      = t_pipeline_info.depth_write_enable,
        .depthCompareOp        = t_pipeline_info.depth_compare_op,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
    };

    VkPipelineColorBlendStateCreateInfo color_blend{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_NO_OP,
        .attachmentCount = static_cast<std::uint32_t>(t_pipeline_info.color_blend_attachments.size()),
        .pAttachments    = t_pipeline_info.color_blend_attachments.data(),
        .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = static_cast<std::uint32_t>(t_pipeline_info.dynamic_states.size()),
        .pDynamicStates    = t_pipeline_info.dynamic_states.data(),
    };


    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = 0U,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0U,
        .pPushConstantRanges    = nullptr,
    };

    result = vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &g_pipeline_layout);
    if (result != VK_SUCCESS)
    {
        LogCritical("Could not create a pipeline layout!");
        return std::unexpected{result};
    }

    VkGraphicsPipelineCreateInfo pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &pipeline_rendering_info,
        .flags               = 0U,
        .stageCount          = 2U,
        .pStages             = shader_stages,
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depth_stencil,
        .pColorBlendState    = &color_blend,
        .pDynamicState       = &dynamic_state,
        // .layout              = g_pipeline_layout,
        // .renderPass          = g_render_pass,
        // .subpass             = 0U,
        // .basePipelineHandle  = VK_NULL_HANDLE,
        // .basePipelineIndex   = -1,
    };

    m_cache.insert({pipeline_key, VK_NULL_HANDLE});
    vkCreateGraphicsPipelines(m_device, m_creation_cache, 1U, &pipeline_info, nullptr, &m_cache[pipeline_key]);

    return m_cache[pipeline_key];
}

}// namespace polos::rendering
