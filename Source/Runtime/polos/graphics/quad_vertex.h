#pragma once

namespace polos
{
struct quad_vertex
{
    glm::vec3 position;
    glm::vec2 texture_coordinates;
    glm::vec4 color;
    int32_t   texture_index{};
    float     tiling_factor{};
};

enum class QuadVertexIndex
{
    k_BottomLeft,
    k_BottomRight,
    k_TopLeft,
    k_TopRight
};
}// namespace polos
