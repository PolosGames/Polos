///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_QUAD_INSTANCE_HPP
#define POLOS_RENDERING_QUAD_INSTANCE_HPP

#include <glm/glm.hpp>

namespace polos::rendering
{

struct QuadInstance
{
    glm::mat4 model{1.0F};
    glm::vec4 color{1.0F, 1.0F, 1.0F, 1.0F};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_QUAD_INSTANCE_HPP
