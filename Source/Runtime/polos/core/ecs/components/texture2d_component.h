#pragma once
#ifndef POLOS_CORE_ECS_COMPONENTS_COMPONENTTEXTURE2D_H_
#define POLOS_CORE_ECS_COMPONENTS_COMPONENTTEXTURE2D_H_

#include "polos/context/texture.h"

namespace polos::ecs
{
    struct texture2d_component
    {
        TextureRef texture;
        glm::vec2 uvCoordinates;
    };
} // namespace polos::ecs

#endif /* POLOS_CORE_ECS_COMPONENTS_COMPONENTTEXTURE2D_H_ */