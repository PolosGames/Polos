///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SCENE_CAMERA3D_HPP
#define POLOS_RENDERING_SCENE_CAMERA3D_HPP

#include "polos/polos_api.hpp"

#include <glm/glm.hpp>

#include <cmath>
#include <cstdint>

namespace polos::rendering
{
struct Camera3D
{
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    glm::vec3 rotation;

    std::float_t nearZ{0.0F};
    std::float_t farZ{0.0F};
    std::int32_t fov{0};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SCENE_CAMERA3D_HPP
