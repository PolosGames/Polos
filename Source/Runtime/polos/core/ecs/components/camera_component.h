#pragma once

#include <glm/glm.hpp>
#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct camera_component final : Component<camera_component>
    {
        // camera Attributes
        glm::vec3 position{};
        glm::vec3 front{};
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};

        // euler Angles
        float yaw{};
        float pitch{};

        // camera options
        float movementSpeed{};
        float mouseSensitivity{};
        float zoom{};
    };

    template<> inline constexpr int32 k_ComponentId<camera_component> = 4;

    template<> void SerializeComponent(std::vector<byte>& p_OutVector, camera_component* p_Component);
    template<> camera_component DeserializeComponent(std::vector<byte>& p_InVector, std::size_t p_Index);
} // namespace polos::ecs
