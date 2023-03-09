#pragma once

#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct info_component final : Component<info_component>
    {
        char name[128];
        bool isSelectedOnEditor{};
        bool isVisible{};
    };

} // namespace polos::ecs
