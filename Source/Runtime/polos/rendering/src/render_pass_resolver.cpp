//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_pass_resolver.hpp"

#include "polos/rendering/compiled_render_pass.hpp"
#include "polos/rendering/render_graph.hpp"

namespace polos::rendering
{

RenderPassResolver::RenderPassResolver(RenderGraph* t_parent_graph, std::size_t t_pass_index)
    : m_parent_graph{t_parent_graph},
      m_pass_index{t_pass_index}
{}

auto RenderPassResolver::Read(RenderGraphResourceHandle t_handle) -> void
{
    compiled_render_pass& pass = m_parent_graph->GetCompiledRenderPass(m_pass_index);
    pass.read_resources.push_back(t_handle);
}

auto RenderPassResolver::Write(attachment_usage const& t_usage) -> void
{
    m_parent_graph->GetCompiledRenderPass(m_pass_index).attachments.push_back(t_usage);
}

}// namespace polos::rendering