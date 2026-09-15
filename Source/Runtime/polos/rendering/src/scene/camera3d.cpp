///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "scene/camera_uniforms.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace polos::rendering
{
auto GetCameraUbo(Camera3D const* tCam, std::uint32_t tScWidth, std::uint32_t tScHeight) -> UniformBufferObject
{
    UniformBufferObject ubo{
        .view = glm::lookAt(tCam->position, tCam->target, tCam->up),
        .proj = glm::perspective(glm::radians(static_cast<std::float_t>(tCam->fov)),
                                 static_cast<float>(tScWidth) / static_cast<float>(tScHeight),
                                 tCam->nearZ,
                                 tCam->farZ)
    };
    ubo.proj[1][1] *= -1;

    return ubo;
}
}// namespace polos::rendering
