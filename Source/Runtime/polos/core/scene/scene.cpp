#include "polos/polos_pch.h"

#include "scene.h"

namespace polos
{
    Scene::Scene()
    {
        // Initialize the free entities array with all the entities array.
        for(int i = 0; i < MAX_ENTITY_COUNT_IN_SCENE ; i++)
        {
            m_FreeEntities.push_back(i);
        }
    }

    ecs::Entity Scene::NewEntity()
    {
        ecs::Entity entt_id;
        ecs::EntityIndex entt_index;

        if (!m_FreeEntities.empty())
        {
            entt_index = m_FreeEntities.back();
            ecs::EntityData& entt_data = m_Entities[entt_index];
            entt_id = ecs::CreateEntity(entt_index, ecs::GetEntityVersion(entt_data.id));

            entt_data.id = entt_id;

            m_FreeEntities.pop_back();
        }
        else
        {
            entt_index = static_cast<ecs::EntityIndex>(m_EntitySize++);
            entt_id = ecs::CreateEntity(entt_index, static_cast<ecs::EntityVersion>(0));
        }

        ecs::EntityData& entt_data = m_Entities[entt_index];
         
        // Lastly set up an empty component mask
        entt_data.mask = ecs::CompMask{};
        
        // Add the info component

        std::string name = "Entity " + std::to_string(entt_index + 1);
        auto* info = Assign<ecs::info_component>(entt_id);
        std::ranges::copy(name, info->name);
        info->isSelectedOnEditor = false;

        return entt_id;
    }

    void Scene::DestroyEntity(ecs::Entity entt)
    {
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(entt);
        ecs::Entity      newID = ecs::CreateEntity(static_cast<ecs::EntityIndex>(-1), ecs::GetEntityVersion(entt) + 1);

        // Invalidate the entity
        m_Entities[entt_index].id = newID;
        m_Entities[entt_index].mask.reset();

        m_FreeEntities.push_back(entt_index);
    }
} // namespace polos
