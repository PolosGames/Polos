#pragma once

#include "polos/core/ecs/common_set.h"
#include "polos/core/ecs/components/info_component.h"

namespace polos::ecs
{
    struct info_set : CommonSet
    {
        COMMONSET_REGISTER(
            info_set,
            ecs::info_component
        )

        info_set(ecs::Entity p_Entity, ecs::info_component* p_Info);
        
        std::string GetEntityName();

        ecs::Entity entity;
        ecs::info_component* infoComponent;
    };
} // namespace polos::ecs
