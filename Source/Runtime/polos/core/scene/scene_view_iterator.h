#pragma once
#ifndef SCENEVIEWITERATOR_H_
#define SCENEVIEWITERATOR_H_

#include "polos/core/scene/base_scene_view.h"
#include "polos/core/ecs/entity.h"

namespace polos
{
    class SceneViewIterator
    {
    public:
        SceneViewIterator(base_scene_view const* view, ecs::EntityIndex index);

        ecs::Entity& operator*() const;

        bool operator==(const SceneViewIterator& other) const;
        bool operator!=(const SceneViewIterator& other) const;

        SceneViewIterator& operator++();
        SceneViewIterator  operator++(int);
    private:
        bool CheckValid();
    private:
        ecs::EntityIndex m_Index{};
        base_scene_view const* m_View;
    };
} // namespace polos

#endif /* SCENEVIEWITERATOR_H_ */