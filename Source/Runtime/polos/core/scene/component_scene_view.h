#pragma once

#include "polos/core/scene/base_scene_view.h"
#include "polos/core/scene/scene_view_iterator.h"
#include "polos/core/ecs/ecs_component.h"

namespace polos
{
    template<ecs::EcsComponent... Components>
    class ComponentSceneView : public BaseSceneView<SceneViewIterator<Components...>>
    {
    public:
        ComponentSceneView() = delete;
        ComponentSceneView(Scene& p_Scene)
        {
            this->scene = &p_Scene;
            constexpr size_t component_count = sizeof...(Components);
            if constexpr (component_count == 0)
            {
                this->iterateAll = true;
            }

            std::array<size_t, component_count> comp_ids = { ecs::Component<Components>::GetId()...};

            for (size_t i{}; i < component_count; i++)
            {
                this->mask.set(static_cast<std::size_t>(comp_ids[i]));
            }
        }

        virtual ~ComponentSceneView() = default;
    };
} // namespace polos
