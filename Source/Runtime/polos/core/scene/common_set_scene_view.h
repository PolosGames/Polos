#pragma once

#include "polos/core/scene/base_scene_view.h"
#include "polos/core/scene/scene_view_iterator.h"

namespace polos
{
    template<ecs::CommonSetLike T>
    class CommonSetSceneView : public BaseSceneView<typename T::IteratorType>
    {
    public:
        CommonSetSceneView() = delete;
        explicit CommonSetSceneView(Scene& p_Scene)
        {
            this->scene = &p_Scene;
            if constexpr (k_ComponentCount == 0)
            {
                this->iterateAll = true;
            }

            constexpr std::array<int32, k_ComponentCount> comp_ids = T::component_ids_v;

            for (std::size_t i{}; i < k_ComponentCount; i++)
            {
                this->mask.set(static_cast<std::size_t>(comp_ids[i]));
            }
        }

        virtual ~CommonSetSceneView() = default;

        static constexpr std::size_t k_ComponentCount = std::tuple_size_v<typename T::IteratorType::Types>;
    };
} // namespace polos
