#pragma once
#ifndef POLOS_CORE_ECS_COMPONENT_H_
#define POLOS_CORE_ECS_COMPONENT_H_

#include "polos/core/ecs/components/components.h"

namespace polos::ecs
{
    template<typename T>
    int32 g_ComponentId = -1;

    template<> int32 g_ComponentId<transform_component> = 0;
    template<> int32 g_ComponentId<texture2d_component> = 1;
    template<> int32 g_ComponentId<info_component>      = 2;

    template<typename T>
    concept EcsComponent = requires { g_ComponentId<T> != -1; };
} // namespace polos::ecs

#endif /* POLOS_CORE_ECS_COMPONENT_H_ */