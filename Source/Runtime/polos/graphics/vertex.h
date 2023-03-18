#pragma once

namespace polos
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoord;
        glm::vec3 color;
    };

    enum class QuadVertexIndex
    {
        k_BottomLeft,
        k_BottomRight,
        k_TopLeft,
        k_TopRight
    };
}
