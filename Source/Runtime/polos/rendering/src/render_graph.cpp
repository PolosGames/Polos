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

namespace polos::rendering
{

RenderGraph::RenderGraph()  = default;
RenderGraph::~RenderGraph() = default;

auto RenderGraph::Create(render_graph_creation_details const& t_details) -> Result<void>
{
    m_device = t_details.device;
}

auto RenderGraph::Destroy() -> Result<void> {}

auto RenderGraph::Compile() -> void
{
    for (std::uint32_t i{0U}; i < m_passes.size(); ++i)
    {
        RenderPassResolver builder{this, i};

        builder.m_pass_index = i;
        m_passes[i]->Setup(builder);
    }
}

auto RenderGraph::Execute(VkCommandBuffer t_cmd_buf) -> void
{
    RenderGraphRegistry registry;
    {
        for (auto const& pass : m_compiled_passes)
        {
            if (!pass.is_graphical)
            {
                continue;
            }

            if (!pass.attachments.empty())
            {
                LogInfo("No attachments in render pass");
                continue;
            }

            std::vector<VkImageView> attachment_views;
            VkExtent2D               root_extent = GetResourceNode(pass.attachments[0].handle).extent;
        }
    }
}

auto RenderGraph::Clear() -> void
{
    m_passes.clear();
}

auto RenderGraph::GetResourceNode(RenderGraphResourceHandle t_handle) -> render_graph_resource_node&
{
    return m_resources[t_handle.Index()];
}

auto RenderGraph::GetCompiledRenderPass(std::size_t t_pass_index) -> compiled_render_pass&
{
    return m_compiled_passes[t_pass_index];
}

auto RenderGraph::ImportTexture(std::string_view t_name, VkImage t_image, VkImageView t_view)
    -> RenderGraphResourceHandle
{
    RenderGraphResourceHandle handle = create_handle(RendererResourceType::kTexture);

    auto& node = m_resources[handle.Index()];

    node.name               = t_name;
    node.handle             = handle;
    node.allocated_resource = t_image;
    node.view               = t_view;
    node.is_imported        = true;

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

auto RenderGraph::create_handle(RendererResourceType t_type) -> RenderGraphResourceHandle
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

}// namespace polos::rendering