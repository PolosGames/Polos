///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_COMPONENT_HPP
#define POLOS_ECS_COMPONENT_HPP

#include "polos/ecs/settings.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace polos::ecs
{

enum class ComponentType : std::uint8_t
{
    kInfo                   = 0U,
    kVisibility             = 1U,
    kCamera                 = 2U,
    kTransform              = 3U,
    kEditorCameraController = 4U,
    kMeshRenderer           = 5U,
    kQuadRenderer           = 6U,

    kMaxComponents = Settings::kMaxComponents,
};

template<typename T>
concept Component = requires {
    { T::kType } -> std::convertible_to<ComponentType>;
    requires(static_cast<std::size_t>(T::kType) < Settings::kMaxComponents);
};

}// namespace polos::ecs

#endif// POLOS_ECS_COMPONENT_HPP
