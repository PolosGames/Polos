#pragma once

#include "polos/core/ecs/ecs_component.h"
#include "polos/graphics/shader.h"

namespace polos::ecs
{
    struct material_component final : Component<material_component>
    {
        Shader* shader;
    };
}// namespace polos
