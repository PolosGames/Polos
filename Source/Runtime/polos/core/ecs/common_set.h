#pragma once

#include "polos/utils/macro_util.h"
#include "polos/core/ecs/components/components.h"
#include "polos/core/scene/scene_view_iterator.h"

namespace polos::ecs
{
    template<ecs::EcsComponent... T >
    struct common_set_component_ids
    {
        static constexpr std::array<int32, sizeof...(T)> value{ k_ComponentId<T>...};
    };

    template<ecs::EcsComponent... T >
    inline constexpr auto common_set_component_ids_v = common_set_component_ids<T...>::value;

    class CommonSet
    {};
} // namespace polos::ecs

#define COMMONSET_REGISTER(Name, ...)                                                               \
    using IteratorType = ::polos::SceneViewIterator<Name, PL_EXPAND(__VA_ARGS__)>; \
    static constexpr auto component_ids_v = common_set_component_ids_v<PL_EXPAND(__VA_ARGS__)>;
