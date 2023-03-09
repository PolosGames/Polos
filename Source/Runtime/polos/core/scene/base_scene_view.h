#pragma once

#include "polos/core/scene/scene.h"
#include "polos/core/ecs/entity.h"

namespace polos
{
    struct base_scene_view
    {
        Scene*        scene{};
        ecs::CompMask mask;
        bool          iterateAll{};
    };
} // namespace polos
