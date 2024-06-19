#pragma once

#include <glm/glm.hpp>
#include "polos/core/ecs/ecs_component.h"
#include "polos/core/camera_type.h"

namespace polos::ecs
{
    struct camera_component final : Component<camera_component>
    {
        bool       isCurrent{};
        bool       isEditor{};
        CameraType type{};

        // camera Attributes
        glm::vec3 front{0.0f, 0.0f, -1.0f};
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};

        // camera options
        float movementSpeed{1.2f};
        float mouseSensitivity{0.1f};
        float zoom{45.0f};
    };

    template<> inline constexpr int32 k_ComponentId<camera_component> = 4;

    template<> void SerializeComponent(std::vector<byte>& p_OutVector, camera_component* p_Component);
    template<> camera_component DeserializeComponent(std::vector<byte>& p_InVector, ptrdiff_t p_Index);
} // namespace polos::ecs
