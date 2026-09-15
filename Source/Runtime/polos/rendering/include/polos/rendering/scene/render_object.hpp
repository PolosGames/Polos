///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SCENE_RENDER_OBJECT_HPP
#define POLOS_RENDERING_SCENE_RENDER_OBJECT_HPP

#include <glm/glm.hpp>

#include <memory>

namespace polos::rendering
{

struct Material;

struct RenderObject
{
    glm::mat4 transform{0.0F};
    glm::vec4 color{.0F, .0F, .0F, 1.0F};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_SCENE_RENDER_OBJECT_HPP
