#pragma once

#include "polos/context/texture.h"
#include "polos/core/ecs/component.h"

namespace polos::ecs
{
    struct texture2d_component
    {
        TextureRef texture;
        glm::vec2 uvCoordinates{};
    };

} // namespace polos::ecs

SET_COMPONENT_ID(polos::ecs::texture2d_component)