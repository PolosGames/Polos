#pragma once

namespace polos::ecs
{
    extern int32 s_ComponentCounter;

    template<typename T>
    int32 g_ComponentId = 0;

    template<typename T>
    int32 GetComponentId()
    {
        if (g_ComponentId<T> != 0)
        {
            return g_ComponentId<T>;
        }
        return s_ComponentCounter++;
    }

    template<typename T>
    concept EcsComponent = requires { g_ComponentId<T> != 0; };
} // namespace polos::ecs

#define SET_COMPONENT_ID(ComponentType) \
    template<>                          \
    polos::int32 polos::ecs::g_ComponentId<ComponentType> = ::polos::ecs::GetComponentId<ComponentType>();
