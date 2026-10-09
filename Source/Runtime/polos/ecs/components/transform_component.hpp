///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_TRANSFORM_COMPONENT_HPP
#define POLOS_ECS_TRANSFORM_COMPONENT_HPP

#include "polos/ecs/component.hpp"

#include <glm/glm.hpp>

namespace polos::ecs
{
struct TransformComponent
{
    static constexpr ComponentType kType = ComponentType::kTransform;

    glm::vec3 position{0.0F, 0.0F, 0.0F};
    glm::vec3 rotation{0.0F, 0.0F, 0.0F};
    glm::vec3 scale{1.0F, 1.0F, 1.0F};
};
}// namespace polos::ecs

#endif// POLOS_ECS_TRANSFORM_COMPONENT_HPP
