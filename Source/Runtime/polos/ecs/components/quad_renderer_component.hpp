///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_QUAD_RENDERER_COMPONENT_HPP
#define POLOS_ECS_QUAD_RENDERER_COMPONENT_HPP

#include "polos/ecs/component.hpp"
#include "polos/utils/string_id.hpp"

#include <glm/glm.hpp>

namespace polos::ecs
{

struct QuadRendererComponent
{
    static constexpr ComponentType kType = ComponentType::kQuadRenderer;

    glm::vec4        color{1.0F, 1.0F, 1.0F, 1.0F};
    glm::vec2        scissor{0.0F, 0.0F};
    utils::string_id textureName{0};
    float            tilingFactor{1.0F};
};

}// namespace polos::ecs

#endif// POLOS_ECS_QUAD_RENDERER_COMPONENT_HPP
