///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_EDITOR_CAMERA_CONTROLLER_COMPONENT_HPP
#define POLOS_ECS_EDITOR_CAMERA_CONTROLLER_COMPONENT_HPP

#include "polos/ecs/component.hpp"

namespace polos::ecs
{
struct EditorCameraControllerComponent
{
    static constexpr ComponentType kType = ComponentType::kEditorCameraController;

    float moveSpeed{5.0F};
    float lookSensitivity{0.1F};
};
}// namespace polos::ecs

#endif// POLOS_ECS_EDITOR_CAMERA_CONTROLLER_COMPONENT_HPP
