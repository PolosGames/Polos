#pragma once

#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct transform_component final : Component<transform_component>
    {
        glm::vec4 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{};
    };
} // namespace polos::ecs
