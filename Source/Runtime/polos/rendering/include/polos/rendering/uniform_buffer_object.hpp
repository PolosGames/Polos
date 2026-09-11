///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_UNIFORM_BUFFER_OBJECT_HPP
#define POLOS_RENDERING_UNIFORM_BUFFER_OBJECT_HPP

#include <glm/glm.hpp>

namespace polos::rendering
{

struct alignas(64) UniformBufferObject
{
    glm::mat4 view;
    glm::mat4 proj;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_UNIFORM_BUFFER_OBJECT_HPP
