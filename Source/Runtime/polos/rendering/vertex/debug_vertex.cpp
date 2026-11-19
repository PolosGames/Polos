///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vertex/debug_vertex.hpp"

namespace polos::rendering
{
auto DebugVertex::CreateVertexInputDescription() -> VertexInputDescription
{
    VertexInputDescription description;

    description.attributes.reserve(2U);

    VkVertexInputBindingDescription const main_binding{
        .binding   = 0U,
        .stride    = sizeof(DebugVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    description.bindings.push_back(main_binding);

    {
        VkVertexInputAttributeDescription const attr{
            .location = 0U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(DebugVertex, position),
        };
        description.attributes.push_back(attr);
    }

    {
        VkVertexInputAttributeDescription const attr{
            .location = 1U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(DebugVertex, color),
        };
        description.attributes.push_back(attr);
    }

    return description;
}
}// namespace polos::rendering
