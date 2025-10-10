//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_

#include "polos/rendering/common.hpp"

#include <string>
#include <variant>

namespace polos::rendering
{

struct render_graph_resource_node
{
    std::string name;

    std::variant<VkImage>     allocated_resource{VK_NULL_HANDLE};
    std::variant<VkImageView> view{VK_NULL_HANDLE};

    bool                       is_imported{false};
    std::uint32_t              producer_pass_index{static_cast<std::uint32_t>(-1)};
    std::vector<std::uint32_t> consumer_pass_indices;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_RESOURCE_NODE_HPP_
