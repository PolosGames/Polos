//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vertex.hpp"

namespace polos::rendering
{

VertexInputDescription CreateVertexDescription(VertexAttributes t_flags)
{
    VertexInputDescription description;

    // --- Binding Description ---
    // We have one binding for the per-vertex data.
    VkVertexInputBindingDescription mainBinding{
        .binding   = 0U,
        .stride    = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    description.bindings.push_back(mainBinding);

    // --- Attribute Descriptions ---
    // We create an attribute description for each active attribute based on the flags.

    if (t_flags & VertexAttributes::kPositionOnly)
    {
        VkVertexInputAttributeDescription attr{
            .location = 0U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, position),
        };
        description.attributes.push_back(attr);
    }

    if (t_flags & VertexAttributes::kWithNormals)
    {
        VkVertexInputAttributeDescription attr{
            .location = 1U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, normal),
        };
        description.attributes.push_back(attr);
    }

    if (t_flags & VertexAttributes::kWithColors)
    {
        VkVertexInputAttributeDescription attr{
            .location = 2U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, color),
        };
        description.attributes.push_back(attr);
    }

    if (t_flags & VertexAttributes::kWithTexCoords)
    {
        VkVertexInputAttributeDescription attr{
            .location = 3U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(Vertex, tex_coord),
        };
        description.attributes.push_back(attr);
    }

    return description;
}

}// namespace polos::rendering