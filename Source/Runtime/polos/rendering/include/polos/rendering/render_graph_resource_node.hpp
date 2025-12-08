///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP
#define POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/texture_2d.hpp"

#include <limits>
#include <memory>
#include <string>

namespace polos::rendering
{

struct alignas(64) render_graph_resource_node// NOLINT
{
    std::string               name;
    RenderGraphResourceHandle handle{RenderGraphResourceHandle::Invalid()};

    std::shared_ptr<texture_2d> raw_resource;

    bool        is_imported{false};
    std::size_t producer_pass_index{std::numeric_limits<std::size_t>::max()};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP
