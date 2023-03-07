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
        SceneView(Scene& scene);

        PL_NODISCARD auto begin() const noexcept -> iterator;
        PL_NODISCARD auto end() const noexcept -> iterator;
    };

    template<typename... Components>
    SceneView<Components...>::SceneView(Scene& scene)
    {
        m_Scene = &scene;
        if constexpr (sizeof...(Components) == 0)
        {
            m_IterateAll = true;
        }

        int comp_ids[] = { ecs::Component<Components>::GetId()...};

        for (int const& id : comp_ids)
        {
            m_Mask.set(id);
        }
    }

    template<typename... Components>
    auto SceneView<Components...>::begin() const noexcept -> iterator
    {
        return iterator{reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(0)};
    }

    template<typename... Components>
    auto SceneView<Components...>::end() const noexcept -> iterator
    {
        return iterator{reinterpret_cast<base_scene_view const*>(this), static_cast<ecs::EntityIndex>(MAX_ENTITY_COUNT_IN_SCENE)};
    }
} // namespace polos
