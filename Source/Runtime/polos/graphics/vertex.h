#pragma once

namespace polos
{
struct quad_vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
    glm::vec4 color;
};

enum class QuadVertexIndex
{
    k_BottomLeft,
    k_BottomRight,
    k_TopLeft,
    k_TopRight
};
}// namespace polos
