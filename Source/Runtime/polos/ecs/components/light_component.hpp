///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_COMPONENTS_LIGHT_COMPONENT_HPP
#define POLOS_ECS_COMPONENTS_LIGHT_COMPONENT_HPP

#include "polos/ecs/component.hpp"

#include <glm/glm.hpp>

namespace polos::ecs
{
struct LightComponent
{
    static constexpr ComponentType kType = ComponentType::kLight;

    glm::vec3 color{1.0F};
    float     ambientFactor{1.0F};
    float     specularFactor{1.0F};
};
}// namespace polos::ecs

#endif// POLOS_ECS_COMPONENTS_LIGHT_COMPONENT_HPP
