//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_graph.hpp"

namespace polos::rendering
{

auto RenderGraph::Compile() -> void
{
    for (std::uint32_t i{0U}; i < m_passes.size(); ++i)
    {
        RenderPassBuilder builder{this, i};

        builder.m_pass_index = i;
        m_passes[i]->Setup(builder);
    }
}

auto RenderGraph::Execute(VkCommandBuffer t_cmd_buf) -> void
{
    RenderPassRegistry registry;
    {
        for (auto const& pass : m_passes) { pass->Execute(t_cmd_buf, registry); }
    }
}

auto RenderGraph::Clear() -> void
{
    m_passes.clear();
}

}// namespace polos::rendering