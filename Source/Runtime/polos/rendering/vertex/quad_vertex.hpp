///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VERTEX_QUAD_VERTEX_HPP
#define POLOS_RENDERING_VERTEX_QUAD_VERTEX_HPP

#include "polos/rendering/vertex/vertex_input_description.hpp"

#include <glm/glm.hpp>

namespace polos::rendering
{
struct QuadVertex
{
    glm::vec3 position{0.0F, 0.0F, 0.0F};
    glm::vec2 texCoord{0.0F, 0.0F};

    static auto CreateVertexInputDescription() -> VertexInputDescription;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_VERTEX_QUAD_VERTEX_HPP
