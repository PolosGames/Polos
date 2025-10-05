//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VERTEX_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VERTEX_HPP_

#include "polos/rendering/common.hpp"

#include <cstddef>
#include <glm/glm.hpp>

#include <cstdint>

namespace polos::rendering
{

/// @brief An enum to specify which parts of the Vertex struct we need.
enum class VertexAttributes : std::uint32_t
{
    kPositionOnly  = 1 << 0,
    kWithNormals   = 1 << 1,
    kWithColors    = 1 << 2,
    kWithTexCoords = 1 << 3,

    Full = kPositionOnly | kWithNormals | kWithColors | kWithTexCoords
};

inline constexpr std::uint32_t operator&(VertexAttributes const t_lhs, VertexAttributes const t_rhs)
{
    return static_cast<std::uint32_t>(t_lhs) & static_cast<std::uint32_t>(t_rhs);
}

inline constexpr VertexAttributes operator|(VertexAttributes const t_lhs, VertexAttributes const t_rhs)
{
    return static_cast<VertexAttributes>(static_cast<std::uint32_t>(t_lhs) & static_cast<std::uint32_t>(t_rhs));
}

/// @brief A struct to hold the generated Vulkan descriptions.
struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription>   bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 tex_coord;
};

VertexInputDescription CreateVertexDescription(VertexAttributes const flags);

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VERTEX_HPP_
