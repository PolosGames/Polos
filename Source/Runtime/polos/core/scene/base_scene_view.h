#pragma once

#include "polos/core/scene/scene.h"
#include "polos/core/ecs/entity.h"

namespace polos
{
    struct base_scene_view
    {
        Scene*   m_Scene{};
        ecs::CompMask m_Mask;
        bool     m_IterateAll{};
    };
} // namespace polos
