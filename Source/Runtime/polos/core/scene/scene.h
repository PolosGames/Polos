#pragma once

#ifndef SCENE_H_
#define SCENE_H_

#include "polos/core/ecs/component_pool.h"
#include "polos/core/ecs/entity.h"
#include "polos/core/ecs/component.h"

namespace polos
{
    class Scene
    {
    public:
        Scene();

        ecs::Entity NewEntity();
        void   DestroyEntity(ecs::Entity entt);

        template<ecs::EcsComponent T, typename... Args>
        T* Assign(ecs::Entity entt, Args&&... args);

        template<ecs::EcsComponent T>
        T* Get(ecs::Entity entt);

        template<ecs::EcsComponent T>
        void Remove(ecs::Entity entt);
    private:
        friend class SceneViewIterator;
        friend class Editor;
        uint32_t                      m_EntitySize{};
        std::vector<ecs::EntityIndex> m_FreeEntities;

        std::array<ecs::EntityData, MAX_ENTITY_COUNT_IN_SCENE>         m_Entities;
        std::array<ecs::ComponentPool, MAX_COMPONENT_COUNT_FOR_ENTITY> m_CompPools;
    };

    template<ecs::EcsComponent T, typename... Args>
    inline T* Scene::Assign(ecs::Entity entt, Args&&... args)
    {
        int              comp_id    = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        if (m_Entities[entt_index].id != entt)
        {
            return nullptr;
        }

        if (m_Entities[entt_index].mask.test(comp_id))
        {
            LOG_ENGINE_WARN("Entity alrady has the requested component, returning null.");
            return nullptr;
        }

        if (!m_CompPools[comp_id].IsInitialized())
        {
            void* where = &m_CompPools[comp_id];
            new (where) ecs::ComponentPool();
            std::launder(reinterpret_cast<ecs::ComponentPool*>(where))->Create<T>();
        }

        auto* comp_ptr = new (m_CompPools[comp_id].Get(entt_index)) T(std::forward<Args>(args)...);

        m_Entities[entt_index].mask.set(comp_id);

        return comp_ptr;
    }

    template<ecs::EcsComponent T>
    inline T* Scene::Get(ecs::Entity entt)
    {
        if (!ecs::IsEntityValid(entt))
        {
            return nullptr;
        }

        int              comp_id    = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        if (m_Entities[entt_index].id != entt)
        {
            LOG_ENGINE_WARN("Provided entity was not found, returning null.");
            return nullptr;
        }

        if (!m_Entities[entt_index].mask.test(comp_id))
        {
            LOG_ENGINE_WARN("Entity doesn't have requested component, returning null.");
            return nullptr;
        }

        auto* pComponent = static_cast<T*>(m_CompPools[comp_id].Get(entt_index));
        return pComponent;
    }

    template<ecs::EcsComponent T>
    inline void Scene::Remove(ecs::Entity entt)
    {
        int         comp_id = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        if (m_Entities[entt_index].id != entt)
        {
            return;
        }

        m_Entities[entt_index].mask.reset(comp_id);
    }
} // namespace polos
#endif /* SCENE_H_ */