#pragma once

#include "polos/core/ecs/component.h"

namespace polos::ecs
{
    struct info_component
    {
        char name[128];
        bool isSelectedOnEditor;
    };

} // namespace polos::ecs

SET_COMPONENT_ID(polos::ecs::info_component)
