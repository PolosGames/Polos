//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/texture_2d.hpp"

#include <limits>
#include <memory>
#include <string>
#include <variant>

namespace polos::rendering
{

struct render_graph_resource_node
{
    std::string               name;
    RenderGraphResourceHandle handle{RenderGraphResourceHandle::Invalid()};

    std::weak_ptr<texture_2d> raw_resource;

    bool        is_imported{false};
    std::size_t producer_pass_index{std::numeric_limits<std::size_t>::max()};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_
