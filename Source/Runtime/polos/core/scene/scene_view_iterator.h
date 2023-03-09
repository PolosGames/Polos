#pragma once

#include "polos/core/scene/base_scene_view.h"
#include "polos/core/ecs/entity.h"

namespace polos
{
    class SceneViewIterator
    {
    public:
        SceneViewIterator(base_scene_view const* p_View, ecs::EntityIndex p_Index);

        ecs::Entity operator*() const;

        bool operator==(const SceneViewIterator& p_Other) const;
        bool operator!=(const SceneViewIterator& p_Other) const;

        SceneViewIterator& operator++();
        SceneViewIterator  operator++(int);
    private:
        bool CheckValid();
    private:
        ecs::EntityIndex m_Index{};
        base_scene_view const* m_View;
    };
} // namespace polos
