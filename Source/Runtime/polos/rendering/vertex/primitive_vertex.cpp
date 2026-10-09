///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vertex/primitive_vertex.hpp"

namespace polos::rendering
{

auto PrimitiveVertex::CreateVertexInputDescription() -> VertexInputDescription
{
    VertexInputDescription description;

    description.attributes.reserve(5U);

    VkVertexInputBindingDescription const main_binding{
        .binding   = 0U,
        .stride    = sizeof(PrimitiveVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    description.bindings.push_back(main_binding);

    {
        VkVertexInputAttributeDescription const attr{
            .location = 0U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(PrimitiveVertex, position),
        };
        description.attributes.push_back(attr);
    }

    {
        VkVertexInputAttributeDescription const attr{
            .location = 1U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(PrimitiveVertex, normal),
        };
        description.attributes.push_back(attr);
    }

    {
        VkVertexInputAttributeDescription const attr{
            .location = 2U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset   = offsetof(PrimitiveVertex, tangent),
        };
        description.attributes.push_back(attr);
    }

    {
        VkVertexInputAttributeDescription const attr{
            .location = 3U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(PrimitiveVertex, texCoord),
        };
        description.attributes.push_back(attr);
    }

    {
        VkVertexInputAttributeDescription const attr{
            .location = 4U,
            .binding  = 0U,
            .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset   = offsetof(PrimitiveVertex, color),
        };
        description.attributes.push_back(attr);
    }

    return description;
}

}// namespace polos::rendering