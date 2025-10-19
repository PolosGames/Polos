//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_graph.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/i_render_pass.hpp"
#include "polos/rendering/render_graph_registry.hpp"
#include "polos/rendering/render_graph_resource_node.hpp"
#include "polos/rendering/render_pass_resolver.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

namespace polos::rendering
{

RenderGraph::RenderGraph()  = default;
RenderGraph::~RenderGraph() = default;

auto RenderGraph::Create(render_graph_creation_details const& t_details) -> Result<void>
{
    m_device = t_details.device;

    return {};
}

auto RenderGraph::Destroy() -> Result<void>
{
    for (auto fb : m_fb_to_destroy) { vkDestroyFramebuffer(m_device, fb, nullptr); }
    for (auto& pass : m_passes) { vkDestroyRenderPass(m_device, pass->vk_pass, nullptr); }
    return {};
}

auto RenderGraph::Compile() -> void
{
    for (std::uint32_t i{0U}; i < m_passes.size(); ++i)
    {
        RenderPassResolver builder{this, i};

        m_compiled_passes.emplace_back();
        auto& compiled_pass = m_compiled_passes[i];

        compiled_pass.name     = m_passes[i]->Name();
        compiled_pass.raw_pass = m_passes[i].get();

        builder.m_pass_index = i;
        m_passes[i]->Setup(builder);
    }
}

auto RenderGraph::Execute(VkCommandBuffer t_cmd_buf) -> void
{
    RenderGraphRegistry registry;
    {
        LogDebug("Compiled passes size: {}, passes size: {}", m_compiled_passes.size(), m_passes.size());
        for (auto const& pass : m_compiled_passes)
        {
            if (!pass.is_graphical)
            {
                continue;
            }

            if (pass.attachments.empty())
            {
                LogInfo("No attachments in render pass");
                continue;
            }

            std::vector<VkImageView> attachment_views;
            VkExtent2D               root_extent;

            VkClearValue clear_color = pass.attachments[0].clear_value;

            if (auto ptr = std::get<0>(GetResourceNode(pass.attachments[0].handle).raw_resource).lock())
            {
                root_extent = ptr->extent;
            }

            for (auto const& attachment : pass.attachments)
            {
                if (auto ptr = std::get<0>(GetResourceNode(attachment.handle).raw_resource).lock())
                {
                    attachment_views.push_back(ptr->view);
                }
            }

            VkFramebufferCreateInfo fb_info{
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext           = nullptr,
                .flags           = 0U,
                .renderPass      = pass.raw_pass->vk_pass,
                .attachmentCount = VK_SIZE_CAST(attachment_views.size()),
                .pAttachments    = attachment_views.data(),
                .width           = root_extent.width,
                .height          = root_extent.height,
                .layers          = 1U,
            };

            VkFramebuffer pass_fb{VK_NULL_HANDLE};
            vkCreateFramebuffer(m_device, &fb_info, nullptr, &pass_fb);

            VkRenderPassBeginInfo pass_begin_info{
                .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext       = nullptr,
                .renderPass  = pass.raw_pass->vk_pass,
                .framebuffer = pass_fb,
                .renderArea =
                    {
                        .offset = {0, 0},
                        .extent = root_extent,
                    },
                .clearValueCount = 1U,
                .pClearValues    = &clear_color,
            };

            vkCmdBeginRenderPass(t_cmd_buf, &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

            pass.raw_pass->Execute(t_cmd_buf, registry);

            vkCmdEndRenderPass(t_cmd_buf);

            m_fb_to_destroy.push_back(pass_fb);
        }
    }
}

auto RenderGraph::Clear() -> void
{
    destroy_transient_resources();
}

auto RenderGraph::GetResourceNode(RenderGraphResourceHandle t_handle) -> render_graph_resource_node&
{
    return m_resources[t_handle.Index()];
}

auto RenderGraph::GetCompiledRenderPass(std::size_t t_pass_index) -> compiled_render_pass&
{
    return m_compiled_passes[t_pass_index];
}

auto RenderGraph::ImportTexture(std::string_view t_name, std::shared_ptr<texture_2d> const& t_texture)
    -> RenderGraphResourceHandle
{
    RenderGraphResourceHandle handle = create_handle(RenderGraphResourceType::kTexture);

    auto& node = m_resources[handle.Index()];

    node.name         = t_name;
    node.handle       = handle;
    node.raw_resource = t_texture;
    node.is_imported  = true;

    return handle;
}

auto RenderGraph::acquire_resource_slot_index() -> std::size_t
{
    std::size_t index{0U};

    if (!m_free_resource_indices.empty())
    {
        index = m_free_resource_indices.back();
        m_free_resource_indices.pop_back();
    }
    else
    {
        index = m_resources.size();
        m_resources.emplace_back();
    }

    return index;
}

auto RenderGraph::create_handle(RenderGraphResourceType t_type) -> RenderGraphResourceHandle
{
    std::uint16_t version{0U};
    std::size_t   index = acquire_resource_slot_index();

    // Fine for both invalid and valid handles, since invalid will just return 0.
    version = m_resources[index].handle.Version();
    return RenderGraphResourceHandle::Create(static_cast<std::uint16_t>(index), t_type, version);
}

auto RenderGraph::destroy_handle(RenderGraphResourceHandle t_handle) -> void
{
    if (!t_handle.IsValid())
    {
        return;
    }

    std::size_t index = t_handle.Index();

    m_free_resource_indices.push_back(index);

    RenderGraphResourceHandle current_handle = m_resources[index].handle;
    m_resources[index].handle                = RenderGraphResourceHandle::Create(
        current_handle.Index16(),
        current_handle.Type(),
        current_handle.Version() + 1);
}

auto RenderGraph::is_handle_valid(RenderGraphResourceHandle t_handle) -> bool
{
    if (!t_handle.IsValid())
    {
        return false;
    }

    std::size_t index = t_handle.Index();
    if (index >= m_resources.size())
    {
        return false;
    }

    return m_resources[index].handle.Version() == t_handle.Version();
}

auto RenderGraph::destroy_transient_resources() -> void
{
    for (auto const& fb : m_fb_to_destroy) { vkDestroyFramebuffer(m_device, fb, nullptr); }
    m_compiled_passes.clear();
    m_passes.clear();
}

}// namespace polos::rendering