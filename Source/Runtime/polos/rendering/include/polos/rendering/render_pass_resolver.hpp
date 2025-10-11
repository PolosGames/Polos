//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_RESOLVER_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_RESOLVER_HPP_

#include "polos/rendering/attachment_info.hpp"
#include "polos/rendering/common.hpp"

namespace polos::rendering
{

class RenderGraph;

/// @brief A temporary builder object passed to an IRenderPass's Setup() method.
/// It provides the interface for a pass to declare its resource usage.
class RenderPassResolver
{
public:
    /// @brief Declares that this pass will read from a resource.
    /// @param handle The handle to the resource being read.
    auto Read(RenderGraphResourceHandle t_handle) -> void;

    /// @brief Declares that this pass will write to a resource.
    /// @param t_attachment_info The attachment information for the resource being written.
    auto Write(attachment_info const& t_attachment_info) -> void;
private:
    friend class RenderGraph;

    explicit RenderPassResolver(RenderGraph* t_parent_graph, std::size_t t_pass_index = 0U);
    ~RenderPassResolver() = default;

    RenderGraph* m_parent_graph{nullptr};
    std::size_t  m_pass_index{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_RESOLVER_HPP_
