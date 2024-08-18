#pragma once

#include "polos/core/scene/base_scene_view.h"
#include "polos/core/ecs/entity.h"
#include "polos/core/ecs/common_set.h"

namespace polos
{
    template<typename... Components> class SceneViewIterator;

    template<ecs::EcsComponent... Components>
    class SceneViewIterator<Components...>
    {
    public:
        SceneViewIterator(BaseSceneView<SceneViewIterator<Components...>> const* p_View, ecs::EntityIndex p_Index)
            : m_Index{p_Index}, m_View{p_View}
        {}

        std::tuple<Components*...> operator*() const
        {
            return {m_View->scene->template Get<Components>(m_View->scene->GetEntityByIndex(m_Index))...};
        }

        bool operator==(const SceneViewIterator& p_Other) const
        {
            return m_Index == p_Other.m_Index || m_Index == m_View->scene->GetEntityPoolSize();
        }

        bool operator!=(const SceneViewIterator& p_Other) const
        {
            return m_Index != p_Other.m_Index && m_Index != m_View->scene->GetEntityPoolSize();
        }

        SceneViewIterator& operator++()
        {
            do
            {
                ++m_Index;
            } while (m_Index < m_View->scene->GetEntityPoolSize() && !CheckValid());

            return *this;
        }

        SceneViewIterator operator++(int)
        {
            auto temp_this = *this;
            ++(*this);
            return temp_this;
        }

    private:
        bool CheckValid()
        {
            auto& mask = m_View->mask;
            auto all = m_View->iterateAll;
            auto& entity_data = m_View->scene->GetEntityDataByIndex(m_Index);

            bool has_correct_mask = (all || (mask == (mask & entity_data.mask)));

            return ecs::IsEntityValid(m_View->scene->GetEntityByIndex(m_Index)) && has_correct_mask;
        }
    private:
        ecs::EntityIndex m_Index{};
        BaseSceneView<SceneViewIterator<Components...>> const* m_View{};
    };

    template<ecs::CommonSetLike T, ecs::EcsComponent... Components>
    class SceneViewIterator<T, Components...>
    {
    public:
        using Types = std::tuple<Components...>;

        SceneViewIterator(BaseSceneView<SceneViewIterator<T, Components...>> const* p_View, ecs::EntityIndex p_Index)
            : m_Index{p_Index}, m_View{p_View}
        {}

        T operator*() const
        {
            ecs::Entity id = m_View->scene->GetEntityByIndex(m_Index);
            T t{id, m_View->scene->template Get<Components>(id)...};

            return t;
        }

        bool operator==(const SceneViewIterator& p_Other) const
        {
            return m_Index == p_Other.m_Index || m_Index == m_View->scene->GetEntityPoolSize();
        }

        bool operator!=(const SceneViewIterator& p_Other) const
        {
            return m_Index != p_Other.m_Index && m_Index != m_View->scene->GetEntityPoolSize();
        }

        SceneViewIterator& operator++()
        {
            do
            {
                ++m_Index;
            } while (m_Index < m_View->scene->GetEntityPoolSize() && !CheckValid());

            return *this;
        }

        SceneViewIterator  operator++(int)
        {
            auto temp_this = *this;
            ++(*this);
            return temp_this;
        }
    private:
        bool CheckValid()
        {
            auto& mask = m_View->mask;
            auto all = m_View->iterateAll;
            auto& entity_data = m_View->scene->GetEntityDataByIndex(m_Index);

            bool has_correct_mask = (all || (mask == (mask & entity_data.mask)));

            return ecs::IsEntityValid(m_View->scene->GetEntityByIndex(m_Index)) && has_correct_mask;
        }
    private:
        ecs::EntityIndex m_Index{};
        BaseSceneView<SceneViewIterator<T, Components...>> const* m_View{};
    };

} // namespace polos
