///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_CAMERA_COMPONENT_HPP
#define POLOS_ECS_CAMERA_COMPONENT_HPP

#include "polos/ecs/component.hpp"

#include <glm/glm.hpp>

#include <cstdint>

namespace polos::ecs
{

enum class CameraType : std::uint8_t
{
    kOrtho,
    kPerspective,
};

struct CameraComponent
{
    static constexpr ComponentType kType = ComponentType::kCamera;

    CameraType cameraType{CameraType::kPerspective};
    float      nearZ{0.0F};
    float      farZ{0.0F};
    float      fov{60.0F}; // perspective
    float      size{10.0F};// ortho
};
}// namespace polos::ecs

#endif// POLOS_ECS_CAMERA_COMPONENT_HPP
