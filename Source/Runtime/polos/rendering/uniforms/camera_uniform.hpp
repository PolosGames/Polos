///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CAMERA_UNIFORM_HPP
#define POLOS_RENDERING_CAMERA_UNIFORM_HPP

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/polos_api.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>

namespace polos::rendering
{

class Buffer;

struct CameraUniform
{ glm::mat4 viewProj; };

auto POLOS_API GetCameraUniform(
    ecs::CameraComponent*    tCamera,
    ecs::TransformComponent* tTransform,
    std::uint32_t            tScWidth,
    std::uint32_t            tScHeight) -> CameraUniform;

auto CreateCameraUniformBuffer() -> std::unique_ptr<Buffer>;
}// namespace polos::rendering

#endif// POLOS_RENDERING_CAMERA_UNIFORM_HPP
