///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PUSH_CONSTANTS_SCENE_LIGHT_INFO_HPP
#define POLOS_RENDERING_PUSH_CONSTANTS_SCENE_LIGHT_INFO_HPP

#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>

namespace polos::rendering
{
struct SceneLightInfo
{
    glm::vec3     color{1.0F};
    float         ambientFactor{1.0F};
    std::uint32_t lightCount{0U};
};
static_assert(offsetof(SceneLightInfo, ambientFactor) == 12U);
static_assert(offsetof(SceneLightInfo, lightCount) == 16U);
static_assert(sizeof(SceneLightInfo) == 20U);

}// namespace polos::rendering

#endif// POLOS_RENDERING_PUSH_CONSTANTS_SCENE_LIGHT_INFO_HPP
