#include "polos/rendering/system/geometry_render_system.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/pipeline_cache.hpp"
#include "polos/rendering/render_context.hpp"
#include "polos/rendering/render_graph.hpp"
#include "polos/rendering/render_graph_registry.hpp"
#include "polos/rendering/render_graph_resource_node.hpp"
#include "polos/rendering/render_pass/lambda_pass.hpp"
#include "polos/rendering/render_pass_resolver.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/shader_cache.hpp"
#include "polos/rendering/vulkan_resource_manager.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

namespace polos::rendering
{

namespace
{
constexpr VkAttachmentReference2 const kColorAttachmentRef{
    .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
    .pNext      = nullptr,
    .attachment = 0U,
    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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
    .pDepthStencilAttachment = nullptr,
    .preserveAttachmentCount = 0U,
    .pPreserveAttachments    = nullptr,
};

constexpr VkSubpassDependency2 const kSubpassDependency{
    .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
    .pNext           = nullptr,
    .srcSubpass      = VK_SUBPASS_EXTERNAL,
    .dstSubpass      = 0U,
    .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .srcAccessMask   = 0U,
    .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .dependencyFlags = 0U,
    .viewOffset      = 0U,
};
}// namespace

GeometryRenderSystem::GeometryRenderSystem(RenderContext& t_context, RenderGraph& t_graph)
    : IRenderSystem(t_context, t_graph)
{}

void GeometryRenderSystem::Initialize()
{
    auto frame_as_texture_res = m_context.GetCurrentFrameTexture();
    if (!frame_as_texture_res.has_value())
    {
        LogError("{}", frame_as_texture_res.error());
        return;
    }

    m_sc_img_handle                        = m_graph.ImportTexture("SwapchainImage", *frame_as_texture_res);
    render_graph_resource_node const& node = m_graph.GetResourceNode(m_sc_img_handle);

    texture_2d const* raw_resource = [&node]() -> texture_2d const* {
        if (nullptr != node.raw_resource)
        {
            return node.raw_resource.get();
        }

        LogError("Could not grab the texture: {}", node.name);
        return nullptr;
    }();

    m_layout.attachments = {
        VkAttachmentDescription2{
            .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .pNext          = nullptr,
            .flags          = 0U,
            .format         = raw_resource->format,
            .samples        = raw_resource->samples,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
    };

    m_layout.subpasses = {kSubpassDesc};

    m_layout.dependencies = {kSubpassDependency};

    render_pass_layout_description const& layout = m_layout;

    {
        auto result = m_context.CreateRenderPass(layout);
        if (!result.has_value())
        {
            LogError("Could not create compatible VkRenderPass.");
            return;
        }

        m_vk_render_pass = *result;
    }

    // TODO(sorbatdev): Create one specific pipeline for now
    {
        shader const& basic_color_sh = m_context.GetShaderCache().GetShaderModule("Basic Color");

        auto result = m_context.GetPipelineCache().ConstructPipeline(
            graphics_pipeline_info{
                .name               = "BasicColorPipeline",
                .vertex_shader      = basic_color_sh.module,
                .fragment_shader    = basic_color_sh.module,
                .topology           = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .vertex_input       = std::nullopt,
                .polygon_mode       = VK_POLYGON_MODE_FILL,
                .cull_mode          = VK_CULL_MODE_BACK_BIT,
                .front_face         = VK_FRONT_FACE_CLOCKWISE,
                .depth_bias_enable  = VK_FALSE,
                .multisampling      = VK_SAMPLE_COUNT_1_BIT,
                .depth_test_enable  = VK_FALSE,
                .depth_write_enable = VK_FALSE,
                .depth_compare_op   = VK_COMPARE_OP_NEVER,
                .color_blend_attachments =
                    {
                        VkPipelineColorBlendAttachmentState{
                            .blendEnable         = VK_FALSE,
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
                    },
                .dynamic_states =
                    {
                        VK_DYNAMIC_STATE_VIEWPORT,
                        VK_DYNAMIC_STATE_SCISSOR,
                    },
                .render_pass = m_vk_render_pass,
                .subpass     = 0U,
            });
        if (!result.has_value())
        {
            LogError("Could not create BasicColorPipeline");
            return;
        }

        m_basic_color_pipeline = result->pipeline;
    }
}

void GeometryRenderSystem::Update()
{
    auto frame_as_texture_res = m_context.GetCurrentFrameTexture();
    if (!frame_as_texture_res.has_value())
    {
        LogError("{}", frame_as_texture_res.error());
        return;
    }

    m_sc_img_handle = m_graph.ImportTexture("SwapchainImage", *frame_as_texture_res);

    // TODO(sorbatdev): Create a new pass each frame for now.
    //       Implement an update function for each individual render pass for later
    IRenderPass* pass = m_graph.AddRenderPass<LambdaPass>(
        [sc_handle = m_sc_img_handle](RenderPassResolver& t_builder) {
            t_builder.Write(
                attachment_usage{
                    .handle      = sc_handle,
                    .load_op     = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .store_op    = VK_ATTACHMENT_STORE_OP_STORE,
                    .clear_value = kPolosClear,
                });
        },
        [pipeline   = m_basic_color_pipeline,
         &swapchain = m_context.GetSwapchain()](VkCommandBuffer t_cmd_buf, RenderGraphRegistry const&) {
            // TODO(sorbatdev): Later use pipeline from material and also shaders
            vkCmdBindPipeline(t_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            vkCmdSetViewport(t_cmd_buf, 0U, 1U, &swapchain.GetViewport());
            vkCmdSetScissor(t_cmd_buf, 0U, 1U, &swapchain.GetScissor());

            for (auto objects : RenderingApi::GetMainScene()->GetObjects()) { vkCmdDraw(t_cmd_buf, 3U, 1U, 0U, 0U); }
        },
        "MainDrawPass");

    pass->vk_pass = m_vk_render_pass;
}

}// namespace polos::rendering
