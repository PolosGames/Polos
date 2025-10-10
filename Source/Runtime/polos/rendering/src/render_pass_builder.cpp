//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_pass_builder.hpp"

namespace polos::rendering
{

RenderPassBuilder::RenderPassBuilder(RenderGraph* t_parent_graph, std::uint32_t t_pass_index = 0U)
    : m_parent_graph{t_parent_graph},
      m_pass_index{t_pass_index}
{}

auto RenderPassBuilder::Read(RendererResourceHandle t_handle) -> void {}

auto RenderPassBuilder::Write(AttachmentInfo const& t_attachment_info) -> void {}

}// namespace polos::rendering