#pragma once

#include "polos/core/ecs/component_pool.h"
#include "polos/core/ecs/entity.h"
#include "polos/core/ecs/ecs_component.h"
#include "polos/core/ecs/components/components.h"

namespace polos
{
    class Scene
    {
    public:
        Scene();

        ecs::Entity NewEntity();
        void   DestroyEntity(ecs::Entity p_Entity);

        template<ecs::EcsComponent T, typename... Args>
        T* Assign(ecs::Entity p_Entity, Args&&... p_Args);

        template<ecs::EcsComponent T>
        T* Get(ecs::Entity p_Entity);

        template<ecs::EcsComponent T>
        bool HasComponent(ecs::Entity p_Entity);

        template<ecs::EcsComponent T>
        void Remove(ecs::Entity p_Entity);

        std::vector<byte> Serialize();
    private:
        friend class SceneViewIterator;
        template<typename...T> friend class SceneView;

        uint32_t                      m_EntitySize{};
        std::vector<ecs::EntityIndex> m_FreeEntities;

        std::array<ecs::EntityData, MAX_ENTITY_COUNT_IN_SCENE>         m_Entities;
        std::array<ecs::ComponentPool, MAX_COMPONENT_COUNT_FOR_ENTITY> m_CompPools;
    };

    template<ecs::EcsComponent T, typename... Args>
    T* Scene::Assign(ecs::Entity p_Entity, Args&&... p_Args)
    {
        if (!ecs::IsEntityValid(p_Entity))
        {
            LOG_ENGINE_ERROR("[Scene::Assign] Provided entity was not valid, returning null.");
            return nullptr;
        }

        int32            comp_id      = ecs::Component<T>::GetId();
        ecs::EntityIndex entity_index = ecs::GetEntityIndex(p_Entity);

        if (m_Entities[entity_index].mask.test(comp_id))
        {
            LOG_ENGINE_WARN("[Scene::Assign] Entity alrady has the requested component, returning the component.");
            return Get<T>(entity_index);
        }

        if (!m_CompPools[comp_id].IsInitialized())
        {
            void* where = &m_CompPools[comp_id];
            new (where) ecs::ComponentPool();
            std::launder(reinterpret_cast<ecs::ComponentPool*>(where))->Create<T>();
        }
        
        auto* comp_ptr = new (m_CompPools[comp_id].Get(entity_index)) T(std::forward<Args>(p_Args)...);
        
        if constexpr (std::is_same_v<T, ecs::texture2d_component>)
        {
            comp_ptr->texture = Texture::Load(std::string_view{});
        }

        m_Entities[entity_index].mask.set(comp_id);

        return comp_ptr;
    }

    template<ecs::EcsComponent T>
    T* Scene::Get(ecs::Entity p_Entity)
    {
        if (!ecs::IsEntityValid(p_Entity))
        {
            LOG_ENGINE_ERROR("[Scene::Get] Provided entity was not valid, returning null.");
            return nullptr;
        }

        int32            comp_id    = ecs::Component<T>::GetId();
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(p_Entity);

        if (m_Entities[entt_index].id != p_Entity)
        {
            LOG_ENGINE_WARN("[Scene::Get] Provided entity was not found, returning null.");
            return nullptr;
        }

        if (!m_Entities[entt_index].mask.test(comp_id))
        {
            LOG_ENGINE_WARN("[Scene::Get] Entity doesn't have \"{}\", returning null.", typeid(T).name());
            return nullptr;
        }

        auto* pComponent = static_cast<T*>(m_CompPools[comp_id].Get(entt_index));
        return pComponent;
    }

    template<ecs::EcsComponent T>
    inline bool Scene::HasComponent(ecs::Entity p_Entity)
    {
        int32            comp_id    = ecs::Component<T>::GetId();
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(p_Entity);

        return m_Entities[entt_index].mask.test(comp_id);
    }

    template<ecs::EcsComponent T>
    inline void Scene::Remove(ecs::Entity p_Entity)
    {
        int32            comp_id    = ecs::Component<T>::GetId();
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(p_Entity);

        if (m_Entities[entt_index].id != p_Entity)
        {
            LOG_ENGINE_WARN("[Scene::Remove] Provided entity was not found, returning null.");
            return;
        }

        m_Entities[entt_index].mask.reset(comp_id);
    }
} // namespace polos
