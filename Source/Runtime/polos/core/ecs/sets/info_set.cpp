
#include "info_set.h"

namespace polos::ecs
{
    info_set::info_set(ecs::Entity p_Entity, ecs::info_component* p_Info)
        : entity{p_Entity}
        , infoComponent{p_Info}
    {

    }

    std::string info_set::GetEntityName()
    {
        return infoComponent->name;
    }
} // namespace polos::ecs