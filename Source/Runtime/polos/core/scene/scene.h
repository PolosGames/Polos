#pragma once

#include "polos/core/ecs/component_pool.h"
#include "polos/core/ecs/entity.h"
#include "polos/core/ecs/component.h"
#include "polos/core/ecs/components/components.h"

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
        bool HasComponent(ecs::Entity entt);

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
    T* Scene::Assign(ecs::Entity entt, Args&&... args)
    {
        int              comp_id    = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        if (m_Entities[entt_index].id != entt)
        {
            return nullptr;
        }

        if (m_Entities[entt_index].mask.test(comp_id))
        {
            LOG_ENGINE_WARN("[Scene::Assign] Entity alrady has the requested component, returning null.");
            return nullptr;
        }

        if (!m_CompPools[comp_id].IsInitialized())
        {
            void* where = &m_CompPools[comp_id];
            new (where) ecs::ComponentPool();
            std::launder(reinterpret_cast<ecs::ComponentPool*>(where))->Create<T>();
        }
        
        auto* comp_ptr = new (m_CompPools[comp_id].Get(entt_index)) T(std::forward<Args>(args)...);
        
        if constexpr (std::is_same_v<T, ecs::texture2d_component>)
        {
            comp_ptr->texture = Texture::Load(std::string_view{});
        }

        m_Entities[entt_index].mask.set(comp_id);

        return comp_ptr;
    }

    template<ecs::EcsComponent T>
    T* Scene::Get(ecs::Entity entt)
    {
        if (!ecs::IsEntityValid(entt))
        {
            return nullptr;
        }

        int              comp_id    = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        if (m_Entities[entt_index].id != entt)
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
    inline bool Scene::HasComponent(ecs::Entity entt)
    {
        int              comp_id = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        return m_Entities[entt_index].mask.test(comp_id);
    }

    template<ecs::EcsComponent T>
    inline void Scene::Remove(ecs::Entity entt)
    {
        int         comp_id = ecs::g_ComponentId<T>;
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);

        if (m_Entities[entt_index].id != entt)
        {
            LOG_ENGINE_WARN("[Scene::Remove] Provided entity was not found, returning null.");
            return;
        }

        m_Entities[entt_index].mask.reset(comp_id);
    }
} // namespace polos
