///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_COMPONENT_HPP
#define POLOS_ECS_COMPONENT_HPP

#include <concepts>
#include <cstdint>

namespace polos::ecs
{

enum class ComponentType : std::uint8_t
{
    kInfo,
    kVisibility,
    kCamera,
    kTransform,
    kEditorCameraController,
    kMeshRenderer,
    kQuadRenderer,

    kMaxComponents,
};

template<typename T>
concept Component = requires {
    { T::kType } -> std::convertible_to<ComponentType>;
};

inline constexpr std::uint8_t kMaxComponents = static_cast<std::uint8_t>(ComponentType::kMaxComponents);

}// namespace polos::ecs

#endif// POLOS_ECS_COMPONENT_HPP
