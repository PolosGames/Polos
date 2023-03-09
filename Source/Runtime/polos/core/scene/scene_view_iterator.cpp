
#include "scene_view_iterator.h"

namespace polos
{
    SceneViewIterator::SceneViewIterator(base_scene_view const* p_View, ecs::EntityIndex p_Index)
        : m_Index{p_Index}, m_View{p_View}
    {}

    ecs::Entity SceneViewIterator::operator*() const
    {
        return(m_View->scene->m_Entities[m_Index].id);
    }

    bool SceneViewIterator::operator==(const SceneViewIterator& p_Other) const
    {
        return m_Index == p_Other.m_Index || m_Index == m_View->scene->m_Entities.size();
    }

    bool SceneViewIterator::operator!=(const SceneViewIterator& p_Other) const
    {
        return m_Index != p_Other.m_Index && m_Index != m_View->scene->m_Entities.size();
    }

    SceneViewIterator& SceneViewIterator::operator++()
    {
        do
        {
            ++m_Index;
        } while (m_Index < m_View->scene->m_Entities.size() && !CheckValid());

        return *this;
    }

    SceneViewIterator SceneViewIterator::operator++(int)
    {
        auto temp_this = *this;
        ++(*this);
        return temp_this;
    }

    bool SceneViewIterator::CheckValid()
    {
        auto& mask = m_View->mask;
        auto& entities = m_View->scene->m_Entities;
        auto& all = m_View->iterateAll;

        bool has_correct_mask = (all || (mask == (mask & entities[m_Index].mask)));

        return ecs::IsEntityValid(entities[m_Index].id) && has_correct_mask;
    }
} // namespace polos

