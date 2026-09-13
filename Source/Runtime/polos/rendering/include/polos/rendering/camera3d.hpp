///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CAMERA3D_HPP
#define POLOS_RENDERING_CAMERA3D_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/uniform_buffer_object.hpp"

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

    std::float_t near_z{0.0F};
    std::float_t far_z{0.0F};
    std::int32_t fov{0};
};

auto GetCameraUbo(Camera3D const* t_cam, std::uint32_t t_sc_width, std::uint32_t t_sc_height) -> UniformBufferObject;

}// namespace polos::rendering

#endif// POLOS_RENDERING_CAMERA3D_HPP
