//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_BUILDER_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_BUILDER_HPP_

#include "polos/rendering/attachment_info.hpp"
#include "polos/rendering/common.hpp"

namespace polos::rendering
{

/// @brief A temporary builder object passed to an IRenderPass's Setup() method.
/// It provides the interface for a pass to declare its resource usage.
class RenderPassBuilder
{
public:
    /// @brief Declares that this pass will read from a resource.
    /// @param handle The handle to the resource being read.
    auto Read(RendererResourceHandle t_handle) -> void;

    /// @brief Declares that this pass will write to a resource.
    /// @param t_attachment_info The attachment information for the resource being written.
    auto Write(AttachmentInfo const& t_attachment_info) -> void;
private:
    friend class RenderGraph;

    explicit RenderPassBuilder(RenderGraph* t_parent_graph, std::uint32_t t_pass_index = 0U);
    ~RenderPassBuilder() = default;

    RenderGraph*  m_parent_graph{nullptr};
    std::uint32_t m_pass_index{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_BUILDER_HPP_
