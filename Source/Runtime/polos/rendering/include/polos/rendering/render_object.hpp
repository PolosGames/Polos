///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_OBJECT_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_OBJECT_HPP

#include <glm/glm.hpp>

#include <memory>

namespace polos::rendering
{

struct material;

struct render_object
{
    glm::mat4                 transform;
    std::shared_ptr<material> render_material;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_OBJECT_HPP
