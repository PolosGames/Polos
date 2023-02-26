#pragma once

namespace polos::ecs
{
    struct transform_component
    {
        glm::vec4 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
} // namespace polos::ecs

SET_COMPONENT_ID(polos::ecs::transform_component)