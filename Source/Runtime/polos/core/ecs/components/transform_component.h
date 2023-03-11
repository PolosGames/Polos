#pragma once

#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct transform_component final : Component<transform_component>
    {
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{};
    };

    template<> void SerializeComponent(std::vector<byte>& p_OutVector, transform_component* p_Component);
    template<> transform_component DeserializeComponent(std::vector<byte>& p_InVector, std::size_t p_Index);

} // namespace polos::ecs
