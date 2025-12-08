///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VERTEX_HPP
#define POLOS_RENDERING_VERTEX_HPP

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace polos::rendering
{

/// @brief An enum to specify which parts of the Vertex struct we need.
enum class VertexAttributes : std::uint32_t// NOLINT
{
    kPositionOnly  = 1U << 0U,
    kWithNormals   = 1U << 1U,
    kWithColors    = 1U << 2U,
    kWithTexCoords = 1U << 3U,

    Full = kPositionOnly | kWithNormals | kWithColors | kWithTexCoords
};

constexpr std::uint32_t operator&(VertexAttributes t_lhs, VertexAttributes t_rhs)
{
    return static_cast<std::uint32_t>(t_lhs) & static_cast<std::uint32_t>(t_rhs);
}

constexpr VertexAttributes operator|(VertexAttributes t_lhs, VertexAttributes t_rhs)
{
    return static_cast<VertexAttributes>(static_cast<std::uint32_t>(t_lhs) & static_cast<std::uint32_t>(t_rhs));
}

/// @brief A struct to hold the generated Vulkan descriptions.
struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription>   bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
};

struct alignas(64) Vertex// NOLINT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 tex_coord;
};

VertexInputDescription CreateVertexDescription(VertexAttributes flags);

}// namespace polos::rendering

#endif// POLOS_RENDERING_VERTEX_HPP
