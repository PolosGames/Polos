#include "polos/polos_pch.h"

#include "scene_view_iterator.h"

namespace polos
{
    SceneViewIterator::SceneViewIterator(base_scene_view const* p_View, ecs::EntityIndex p_Index)
        : m_Index{p_Index}, m_View{p_View}
    {

    }

    ecs::Entity& SceneViewIterator::operator*() const
    {
        return(m_View->m_Scene->m_Entities[m_Index].id);
    }

    bool SceneViewIterator::operator==(const SceneViewIterator& p_Other) const
    {
        return m_Index == p_Other.m_Index || m_Index == m_View->m_Scene->m_Entities.size();
    }

    bool SceneViewIterator::operator!=(const SceneViewIterator& p_Other) const
    {
        return m_Index != p_Other.m_Index && m_Index != m_View->m_Scene->m_Entities.size();
    }

    SceneViewIterator& SceneViewIterator::operator++()
    {
        do
        {
            ++m_Index;
        } while (m_Index < m_View->m_Scene->m_Entities.size() && !CheckValid());

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
        auto& mask = m_View->m_Mask;
        auto& entities = m_View->m_Scene->m_Entities;
        auto& all = m_View->m_IterateAll;

        return ecs::IsEntityValid(entities[m_Index].id) &&
               // It has the correct component mask
               (all || (mask == (mask & entities[m_Index].mask)));
    }
} // namespace polos

