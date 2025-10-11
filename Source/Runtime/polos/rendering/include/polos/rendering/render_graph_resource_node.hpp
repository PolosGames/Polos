//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_

#include "polos/rendering/common.hpp"

#include <limits>
#include <string>
#include <variant>

namespace polos::rendering
{

struct render_graph_resource_node
{
    std::string            name;
    RendererResourceHandle handle{RendererResourceHandle::Invalid()};

    // TODO: Add VkBuffer
    std::variant<VkImage> allocated_resource{VK_NULL_HANDLE};

    // TODO: Add VkBufferView
    std::variant<VkImageView> view{VK_NULL_HANDLE};

    bool        is_imported{false};
    std::size_t producer_pass_index{std::numeric_limits<std::size_t>::max()};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_
