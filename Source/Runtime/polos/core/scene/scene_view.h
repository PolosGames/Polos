#pragma once

#include "polos/core/scene/scene.h"
#include "polos/core/ecs/entity.h"
#include "polos/core/scene/base_scene_view.h"
#include "polos/core/scene/scene_view_iterator.h"
#include "polos/core/ecs/ecs_component.h"

namespace polos
{
    template<typename... Components>
    class SceneView : public base_scene_view
    {
        using iterator = SceneViewIterator;
        using const_iterator = iterator const;
    public:
        SceneView(Scene& p_Scene);

        PL_NODISCARD auto begin() noexcept -> iterator;
        PL_NODISCARD auto end() noexcept -> iterator;

        PL_NODISCARD auto begin() const noexcept -> const_iterator;
        PL_NODISCARD auto end() const noexcept -> const_iterator;

        PL_NODISCARD auto cbegin() const noexcept -> const_iterator;
        PL_NODISCARD auto cend() const noexcept -> const_iterator;
    };

    template<typename... Components>
    SceneView<Components...>::SceneView(Scene& p_Scene)
    {
        scene = &p_Scene;
        constexpr std::size_t component_count = sizeof...(Components);
        if constexpr (component_count == 0)
        {
            iterateAll = true;
        }

        std::array<int32, component_count> comp_ids = {ecs::Component<Components>::GetId()...};

        for (std::size_t i{}; i < component_count; i++)
        {
            mask.set(comp_ids[i]);
        }
    }

    template<typename... Components>
    auto SceneView<Components...>::begin() noexcept -> iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        std::size_t i{};
        while (
            i < scene->m_Entities.size() &&
            (mask != (mask & scene->m_Entities[i].mask) || !ecs::IsEntityValid(scene->m_Entities[i].id)))
        {
            i++;
        }
        
        return iterator(reinterpret_cast<base_scene_view*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<typename... Components>
    auto SceneView<Components...>::end() noexcept -> iterator
    {
        return iterator(reinterpret_cast<base_scene_view*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE));
    }

    template<typename... Components>
    auto SceneView<Components...>::begin() const noexcept -> const_iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        std::size_t i{};
        while (
            i < scene->m_Entities.size() &&
            (mask != (mask & scene->m_Entities[i].mask) || !ecs::IsEntityValid(scene->m_Entities[i].id)))
        {
            i++;
        }
        return const_iterator(reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(i));
    }
    
    template<typename... Components>
    auto SceneView<Components...>::end() const noexcept -> const_iterator
    {
        return const_iterator(reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE));
    }

    template<typename... Components>
    auto SceneView<Components...>::cbegin() const noexcept -> const_iterator
    {
        return begin();
    }

    template<typename... Components>
    auto SceneView<Components...>::cend() const noexcept -> const_iterator
    {
        return end();
    }
} // namespace polos
