///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SSBO_LIGHT_INSTANCE_HPP
#define POLOS_RENDERING_SSBO_LIGHT_INSTANCE_HPP

#include <glm/glm.hpp>

#include <cstddef>

namespace polos::rendering
{
struct LightInstance
{
    glm::vec3 position;
    float     ambientFactor{1.0F};
    glm::vec3 color{1.0F};
    float     specularFactor{1.0F};
};
static_assert(offsetof(LightInstance, ambientFactor) == 12U);
static_assert(offsetof(LightInstance, color) == 16U);
static_assert(offsetof(LightInstance, specularFactor) == 28U);
static_assert(sizeof(LightInstance) == 32U);

}// namespace polos::rendering

#endif// POLOS_RENDERING_SSBO_LIGHT_INSTANCE_HPP
