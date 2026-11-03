///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_COMPONENT_TYPES_HPP
#define POLOS_ECS_COMPONENT_TYPES_HPP

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/quad_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/components/visibility_component.hpp"

namespace polos::ecs
{

template<ComponentType T>
struct ComponentEnumToType;

template<>
struct ComponentEnumToType<ComponentType::kInfo>
{
    using Type = InfoComponent;
};

template<>
struct ComponentEnumToType<ComponentType::kVisibility>
{
    using Type = VisibilityComponent;
};

template<>
struct ComponentEnumToType<ComponentType::kCamera>
{
    using Type = CameraComponent;
};

template<>
struct ComponentEnumToType<ComponentType::kTransform>
{
    using Type = TransformComponent;
};

template<>
struct ComponentEnumToType<ComponentType::kEditorCameraController>
{
    using Type = EditorCameraControllerComponent;
};

template<>
struct ComponentEnumToType<ComponentType::kQuadRenderer>
{
    using Type = QuadRendererComponent;
};

}// namespace polos::ecs

#endif// POLOS_ECS_COMPONENT_TYPES_HPP
