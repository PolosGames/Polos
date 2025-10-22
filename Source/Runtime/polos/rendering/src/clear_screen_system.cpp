//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/system/clear_screen_system.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/render_context.hpp"
#include "polos/rendering/render_graph.hpp"
#include "polos/rendering/render_graph_resource_node.hpp"
#include "polos/rendering/render_pass/clear_screen_pass.hpp"

namespace polos::rendering
{

namespace
{
constexpr VkAttachmentReference2 const color_attachment_ref{
    .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
    .pNext      = nullptr,
    .attachment = 0U,
    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
};
}// namespace

ClearScreenSystem::ClearScreenSystem(RenderContext& t_context, RenderGraph& t_graph)
    : IRenderSystem(t_context, t_graph)
{}

void ClearScreenSystem::Initialize()
{
    auto frame_as_texture_res = m_context.GetCurrentFrameTexture();
    if (!frame_as_texture_res.has_value())
    {
        LogError("Could not acquire current frame as a texture. Skipping ClearScreenSystem::Update");
        return;
    }

    m_sc_img_handle                        = m_graph.ImportTexture("SwapchainImage", *frame_as_texture_res);
    render_graph_resource_node const& node = m_graph.GetResourceNode(m_sc_img_handle);

    texture_2d const* raw_resource = [&node]() -> texture_2d const* {
        if (auto ptr = node.raw_resource.lock())
        {
            return ptr.get();
        }

        LogError("Could not grab the texture: {}", node.name);
        return nullptr;
    }();

    m_layout.attachments = {VkAttachmentDescription2{
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
    }};

    m_layout.subpasses = {VkSubpassDescription2{
        .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .viewMask                = 0U,
        .inputAttachmentCount    = 0U,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1U,
        .pColorAttachments       = &color_attachment_ref,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0U,
        .pPreserveAttachments    = nullptr,
    }};

    m_layout.dependencies = {VkSubpassDependency2{
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
    }};

    render_pass_layout_description const& layout = m_layout;

    auto result = m_context.CreateRenderPass(layout);
    if (!result.has_value())
    {
        LogError("Could not create compatible VkRenderPass.");
        return;
    }

    m_vk_render_pass = *result;
}

void ClearScreenSystem::Update()
{
    auto frame_as_texture_res = m_context.GetCurrentFrameTexture();
    if (!frame_as_texture_res.has_value())
    {
        LogError("Could not acquire current frame as a texture. Skipping ClearScreenSystem::Update");
        return;
    }

    m_sc_img_handle = m_graph.ImportTexture("SwapchainImage", *frame_as_texture_res);

    auto* pass    = m_graph.AddRenderPass<ClearScreenPass>(m_sc_img_handle);
    pass->vk_pass = m_vk_render_pass;
}

}// namespace polos::rendering
