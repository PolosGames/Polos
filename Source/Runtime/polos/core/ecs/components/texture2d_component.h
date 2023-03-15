#pragma once

#include "polos/graphics/texture.h"
#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct texture2d_component final : Component<texture2d_component>
    {
        TextureRef texture;
        glm::vec2  uvBottomLeft  = glm::vec2(0.0f, 0.0f);   
        glm::vec2  uvBottomRight = glm::vec2(1.0f, 0.0f);  
        glm::vec2  uvTopRight    = glm::vec2(1.0f, 1.0f);     
        glm::vec2  uvTopLeft     = glm::vec2(0.0f, 1.0f);      
        bool       hasUvChanged{};
    };

    template<> inline constexpr int32 k_ComponentId<texture2d_component> = 2;
} // namespace polos::ecs
