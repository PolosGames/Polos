//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vertex.hpp"

#include "polos/logging/log_macros.hpp"

namespace polos::rendering
{

VertexInputDescription CreateVertexDescription(VertexAttributes t_flags)
{
    VertexInputDescription description;

    description.attributes.reserve(4U);

    VkVertexInputBindingDescription const main_binding{
        .binding   = 0U,
        .stride    = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    description.bindings.push_back(main_binding);

    if (0U != (t_flags & VertexAttributes::kWithPosition))
    {
        VkVertexInputAttributeDescription const attr{
            .location = 0U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, position),
        };
        description.attributes.push_back(attr);
        LogDebug("Position attribute added at location {}", attr.location);
    }

    if (0U != (t_flags & VertexAttributes::kWithNormals))
    {
        VkVertexInputAttributeDescription const attr{
            .location = 1U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, normal),
        };
        description.attributes.push_back(attr);
        LogDebug("Normal attribute added at location {}", attr.location);
    }

    if (0U != (t_flags & VertexAttributes::kWithColors))
    {
        VkVertexInputAttributeDescription const attr{
            .location = 2U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, color),
        };
        description.attributes.push_back(attr);
        LogDebug("Color attribute added at location {}", attr.location);
    }

    if (0U != (t_flags & VertexAttributes::kWithTexCoords))
    {
        VkVertexInputAttributeDescription const attr{
            .location = 3U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(Vertex, tex_coord),
        };
        description.attributes.push_back(attr);
        LogDebug("TexCoord attribute added at location {}", attr.location);
    }

    return description;
}

}// namespace polos::rendering