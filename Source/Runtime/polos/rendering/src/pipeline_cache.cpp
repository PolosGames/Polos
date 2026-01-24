//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/pipeline_cache.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/graphics_pipeline_info.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
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

auto PipelineCache::GetPipeline(utils::string_id t_pipeline_name) const -> Result<vulkan_pipeline>
{
    auto const itr = m_cache.find(t_pipeline_name);
    if (itr == m_cache.end())
    {
        return ErrorType{RenderingErrc::kFailedFindPipeline};
    };

    return itr->second;
}

auto PipelineCache::GetPipeline(std::string_view const t_pipeline_name) const -> Result<vulkan_pipeline>
{
    return GetPipeline(utils::StrHash64(t_pipeline_name));
}

auto PipelineCache::ConstructPipeline(graphics_pipeline_info const& t_pipeline_info) -> Result<vulkan_pipeline>
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
    //         info.initialDataSize = res->uncompressed_size;
    //         info.pInitialData    = res->data.data();
    //     }

    //     if (vkCreatePipelineCache(m_device, &info, nullptr, &m_creation_cache) != VK_SUCCESS)
    //     {
    //         LogWarn("Something went wrong while creating VkPipelineCache.");
    //     }
    // }// END pipeline cache create

    utils::string_id const pipeline_key = t_pipeline_info.name;

    auto const itr = m_cache.find(pipeline_key);
    if (itr != m_cache.end())
    {
        LogWarn("You are trying to create a pipeline that already exists in the cache: |{}|", t_pipeline_info.name);
        return itr->second;
    }

    VkPipelineRenderingCreateInfo const pipeline_rendering_info{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0U,
        .colorAttachmentCount    = 1U,
        .pColorAttachmentFormats = &m_swapchain->GetSurfaceFormat().format,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages(t_pipeline_info.shaders.size());
    for (std::size_t i{0U}; i < t_pipeline_info.shaders.size(); ++i)
    {
        shader const*                   shader = t_pipeline_info.shaders[i];
        VkPipelineShaderStageCreateInfo shader_stage_info{
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stage               = static_cast<VkShaderStageFlagBits>(shader->stage),
            .module              = shader->module,
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
    if (t_pipeline_info.vertex_input.has_value())
    {
        vertex_input_info.vertexBindingDescriptionCount = VK_SIZE_CAST(t_pipeline_info.vertex_input->bindings.size());
        vertex_input_info.pVertexBindingDescriptions    = t_pipeline_info.vertex_input->bindings.data();
        vertex_input_info.vertexAttributeDescriptionCount =
            VK_SIZE_CAST(t_pipeline_info.vertex_input->attributes.size());
        vertex_input_info.pVertexAttributeDescriptions = t_pipeline_info.vertex_input->attributes.data();
    }

    VkPipelineInputAssemblyStateCreateInfo const input_assembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .topology               = t_pipeline_info.topology,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo const viewport_state{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .viewportCount = 1U,
        .pViewports    = &m_swapchain->GetViewport(),
        .scissorCount  = 1U,
        .pScissors     = &m_swapchain->GetScissor(),
    };

    VkPipelineRasterizationStateCreateInfo const rasterizer{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = t_pipeline_info.polygon_mode,
        .cullMode                = t_pipeline_info.cull_mode,
        .frontFace               = t_pipeline_info.front_face,
        .depthBiasEnable         = t_pipeline_info.depth_bias_enable,
        .depthBiasConstantFactor = 0.0F,
        .depthBiasClamp          = 0.0F,
        .depthBiasSlopeFactor    = 0.0F,
        .lineWidth               = 5.0F,
    };

    VkPipelineMultisampleStateCreateInfo const multisampling{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .rasterizationSamples  = t_pipeline_info.multisampling,
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
        .depthTestEnable       = t_pipeline_info.depth_test_enable,
        .depthWriteEnable      = t_pipeline_info.depth_write_enable,
        .depthCompareOp        = t_pipeline_info.depth_compare_op,
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
        .attachmentCount = VK_SIZE_CAST(t_pipeline_info.color_blend_attachments.size()),
        .pAttachments    = t_pipeline_info.color_blend_attachments.data(),
        .blendConstants  = {0.0F, 0.0F, 0.0F, 0.0F},
    };

    VkPipelineDynamicStateCreateInfo const dynamic_state{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0U,
        .dynamicStateCount = VK_SIZE_CAST(t_pipeline_info.dynamic_states.size()),
        .pDynamicStates    = t_pipeline_info.dynamic_states.data(),
    };

    VkDescriptorSetLayoutCreateInfo const desc_set_layout_info{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0U,
        .bindingCount = VK_SIZE_CAST(t_pipeline_info.descriptor_set_layout_binding.size()),
        .pBindings    = t_pipeline_info.descriptor_set_layout_binding.data(),
    };

    VkDescriptorSetLayout desc_set_layout{VK_NULL_HANDLE};
    assert(VK_SUCCESS == vkCreateDescriptorSetLayout(m_device, &desc_set_layout_info, nullptr, &desc_set_layout));

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
    assert(VK_SUCCESS == vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &pipeline_layout));

    VkGraphicsPipelineCreateInfo const pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &pipeline_rendering_info,
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
        .renderPass          = t_pipeline_info.render_pass,
        .subpass             = t_pipeline_info.subpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    VkPipeline pipeline{VK_NULL_HANDLE};
    if (vkCreateGraphicsPipelines(m_device, m_creation_cache, 1U, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kFailedCreatePipeline};
    }

    auto [itr_inserted, was_inserted] = m_cache.insert(
        std::make_pair(
            pipeline_key,
            vulkan_pipeline{
                .pipeline               = pipeline,
                .layout                 = pipeline_layout,
                .descriptor_set_layouts = std::vector(descriptor_set_layouts.begin(), descriptor_set_layouts.end()),
            }));

    return itr_inserted->second;
}

auto PipelineCache::Destroy() -> Result<void>
{
    LogInfo("Destroying and invalidating pipeline cache...");
    for (auto const& [_, pipeline] : m_cache)// NOLINT
    {
        if (VK_NULL_HANDLE != pipeline.pipeline)
        {
            vkDestroyPipeline(m_device, pipeline.pipeline, nullptr);
        }
        if (VK_NULL_HANDLE != pipeline.layout)
        {
            vkDestroyPipelineLayout(m_device, pipeline.layout, nullptr);
        }
        if (!pipeline.descriptor_set_layouts.empty())
        {
            for (auto const& desc_set_layout : pipeline.descriptor_set_layouts)
            {
                if (VK_NULL_HANDLE != desc_set_layout)
                {
                    vkDestroyDescriptorSetLayout(m_device, desc_set_layout, nullptr);
                }
            }
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

}// namespace polos::rendering
