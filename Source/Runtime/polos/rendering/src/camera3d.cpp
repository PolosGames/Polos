///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/camera3d.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace polos::rendering
{
auto GetCameraUbo(Camera3D const* t_cam, std::uint32_t t_sc_width, std::uint32_t t_sc_height) -> UniformBufferObject
{
    UniformBufferObject ubo{
        .view = glm::lookAt(t_cam->position, t_cam->target, t_cam->up),
        .proj = glm::perspective(glm::radians(static_cast<std::float_t>(t_cam->fov)),
                                 static_cast<float>(t_sc_width) / static_cast<float>(t_sc_height),
                                 t_cam->near_z,
                                 t_cam->far_z)
    };
    ubo.proj[1][1] *= -1;

    return ubo;
}
}// namespace polos::rendering
