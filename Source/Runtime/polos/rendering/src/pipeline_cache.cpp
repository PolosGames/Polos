//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/pipeline_cache.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/graphics_pipeline_info.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

namespace polos::rendering
{

PipelineCache::PipelineCache()  = default;
PipelineCache::~PipelineCache() = default;

auto PipelineCache::Create(pipeline_cache_create_details const& t_details) -> Result<void>
{
    m_device    = t_details.logi_device;
    m_swapchain = t_details.swapchain;

    return {};
}

auto PipelineCache::Destroy() -> Result<void>
{
    for (auto const& [_, pipeline] : m_cache)
    {
        if (pipeline.pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(m_device, pipeline.pipeline, nullptr);
        }
        if (pipeline.layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(m_device, pipeline.layout, nullptr);
        }
    }
    m_cache.clear();

    if (m_creation_cache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(m_device, m_creation_cache, nullptr);
        m_creation_cache = VK_NULL_HANDLE;
    }

    return {};
}


auto PipelineCache::GetPipeline(utils::string_id const t_pipeline_name) const -> Result<vulkan_pipeline>
{
    auto it = m_cache.find(t_pipeline_name);
    if (it == m_cache.end())
    {
        return ErrorType{RenderingErrc::kPipelineNotFoundInCache};
    };

    return it->second;
}

auto PipelineCache::GetPipeline(std::string_view const t_pipeline_name) const -> Result<vulkan_pipeline>
{
    return GetPipeline(utils::StrHash64(t_pipeline_name));
}

auto PipelineCache::LoadOrCreatePipeline(graphics_pipeline_info const& t_pipeline_info) -> Result<vulkan_pipeline>
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

    utils::string_id pipeline_key = utils::StrHash64(std::string_view{t_pipeline_info.name});

    VkPipelineRenderingCreateInfo pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &m_swapchain->GetSurfaceFormat().format,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    VkPipelineShaderStageCreateInfo shader_stages[]{
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
        .vertexBindingDescriptionCount   = VK_SIZE_CAST(t_pipeline_info.vertex_input.bindings.size()),
        .pVertexBindingDescriptions      = t_pipeline_info.vertex_input.bindings.data(),
        .vertexAttributeDescriptionCount = VK_SIZE_CAST(t_pipeline_info.vertex_input.attributes.size()),
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
        .attachmentCount = VK_SIZE_CAST(t_pipeline_info.color_blend_attachments.size()),
        .pAttachments    = t_pipeline_info.color_blend_attachments.data(),
        .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = VK_SIZE_CAST(t_pipeline_info.dynamic_states.size()),
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

    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};

    if (vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kFailedToCreatePipelineLayout};
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
        .layout              = pipeline_layout,
        .renderPass          = t_pipeline_info.render_pass,
        .subpass             = t_pipeline_info.subpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    VkPipeline pipeline{VK_NULL_HANDLE};
    if (vkCreateGraphicsPipelines(
            m_device,
            m_creation_cache,
            1U,
            &pipeline_info,
            nullptr,
            &m_cache[pipeline_key].pipeline) != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kFailedToCreatePipeline};
    }

    m_cache.insert({pipeline_key, {pipeline, pipeline_layout}});

    return m_cache[pipeline_key];
}

}// namespace polos::rendering
