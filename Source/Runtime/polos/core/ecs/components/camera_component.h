#pragma once

#include <glm/glm.hpp>
#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct camera_component final : Component<camera_component>
    {
        // camera Attributes
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;

        // euler Angles
        float yaw;
        float pitch;

        // camera options
        float movementSpeed;
        float mouseSensitivity;
        float zoom;
    };

} // namespace polos::ecs
