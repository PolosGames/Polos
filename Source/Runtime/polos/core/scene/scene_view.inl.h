#pragma once

namespace polos
{
    template<ecs::EcsComponent... Components>
    SceneView<Components...>::SceneView(Scene& p_Scene)
    {
        scene = &p_Scene;
        constexpr size_t component_count = sizeof...(Components);
        if constexpr (component_count == 0)
        {
            iterateAll = true;
        }

        std::array<size_t, component_count> comp_ids = { ecs::Component<Components>::GetId()...};

        for (size_t i{}; i < component_count; i++)
        {
            mask.set(comp_ids[i]);
        }
    }

    template<ecs::EcsComponent... Components>
    auto SceneView<Components...>::begin() noexcept -> iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        uint32_t i{};
        while (
            i < scene->GetEntityPoolSize() &&
            (mask != (mask & scene->GetEntityDataByIndex(i).mask) || !ecs::IsEntityValid(scene->GetEntityByIndex(i))))
        {
            i++;
        }

        return iterator(reinterpret_cast<base_scene_view*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<ecs::EcsComponent... Components>
    auto SceneView<Components...>::end() noexcept -> iterator
    {
        return iterator(reinterpret_cast<base_scene_view*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE));
    }

    template<ecs::EcsComponent... Components>
    auto SceneView<Components...>::begin() const noexcept -> const_iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        std::size_t i{};
        while (
            i < scene->GetEntityPoolSize() &&
            (mask != (mask & scene->GetEntityDataByIndex(i).mask) || !ecs::IsEntityValid(scene->GetEntityByIndex(i))))
        {
            i++;
        }
        return const_iterator(reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<ecs::EcsComponent... Components>
    auto SceneView<Components...>::end() const noexcept -> const_iterator
    {
        return const_iterator(reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE));
    }

    template<ecs::EcsComponent... Components>
    auto SceneView<Components...>::cbegin() const noexcept -> const_iterator
    {
        return begin();
    }

    template<ecs::EcsComponent... Components>
    auto SceneView<Components...>::cend() const noexcept -> const_iterator
    {
        return end();
    }

    //
    // Scene View implementations that accept CommonSet classes
    //

    template<ecs::CommonSetLike T>
    SceneView<T>::SceneView(Scene& p_Scene)
    {
        scene = &p_Scene;
        if constexpr (k_ComponentCount == 0)
        {
            iterateAll = true;
        }

        constexpr std::array<int32, k_ComponentCount> comp_ids = T::component_ids_v;

        for (std::size_t i{}; i < k_ComponentCount; i++)
        {
            mask.set(comp_ids[i]);
        }
    }

    template<ecs::CommonSetLike T>
    PL_NODISCARD auto SceneView<T>::begin() noexcept -> iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        uint32_t i{};
        while (
            i < scene->GetEntityPoolSize() &&
            (mask != (mask & scene->GetEntityDataByIndex(i).mask) || !ecs::IsEntityValid(scene->GetEntityByIndex(i))))
        {
            i++;
        }

        return iterator(reinterpret_cast<base_scene_view*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<ecs::CommonSetLike T>
    PL_NODISCARD auto SceneView<T>::end() noexcept -> iterator
    {
        return iterator(reinterpret_cast<base_scene_view*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE));
    }

    template<ecs::CommonSetLike T>
    PL_NODISCARD auto SceneView<T>::begin() const noexcept -> const_iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        std::size_t i{};
        while (
            i < scene->GetEntityPoolSize() &&
            (mask != (mask & scene->GetEntityDataByIndex(i).mask) || !ecs::IsEntityValid(scene->GetEntityByIndex(i))))
        {
            i++;
        }
        return const_iterator(reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<ecs::CommonSetLike T>
    PL_NODISCARD auto SceneView<T>::end() const noexcept -> const_iterator
    {
        return const_iterator(reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE));
    }

    template<ecs::CommonSetLike T>
    PL_NODISCARD auto SceneView<T>::cbegin() const noexcept -> const_iterator
    {
        begin();
    }

    template<ecs::CommonSetLike T>
    PL_NODISCARD auto SceneView<T>::cend() const noexcept -> const_iterator
    {
        end();
    }
} // namespace polos