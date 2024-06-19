#pragma once

#include "polos/utils/macro_util.h"
#include "polos/core/ecs/components/components.h"

namespace polos::ecs
{
    class CommonSet
    {};

    template<typename T>
    concept CommonSetLike = std::is_base_of_v<CommonSet, T>;
    
    template<ecs::EcsComponent... T >
    struct common_set_component_ids
    {
        static constexpr std::array<int32, sizeof...(T)> value{ k_ComponentId<T>...};
    };

    template<ecs::EcsComponent... T >
    inline constexpr auto common_set_component_ids_v = common_set_component_ids<T...>::value;
} // namespace polos::ecs

#define COMMONSET_REGISTER(Name, ...)                                              \
    using IteratorType = ::polos::SceneViewIterator<Name, __VA_ARGS__>; \
    static constexpr auto component_ids_v = common_set_component_ids_v<__VA_ARGS__>;
