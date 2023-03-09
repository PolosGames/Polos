
#include "scene.h"

namespace polos
{
    Scene::Scene()
        : m_EntitySize{5}
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

    void Scene::DestroyEntity(ecs::Entity p_Entity)
    {
        ecs::EntityIndex entt_index = ecs::GetEntityIndex(p_Entity);
        ecs::Entity      newID = ecs::CreateEntity(static_cast<ecs::EntityIndex>(-1), ecs::GetEntityVersion(p_Entity) + 1);

        // Invalidate the entity
        m_Entities[entt_index].id = newID;
        m_Entities[entt_index].mask.reset();

        m_FreeEntities.push_back(entt_index);
    }

    std::vector<byte> Scene::Serialize()
    {
        std::span<ecs::EntityIndex>   free_entities(m_FreeEntities);
        std::span<ecs::EntityData>    entities(m_Entities);
        std::span<ecs::ComponentPool> component_pools(m_CompPools);

        std::vector<byte> scene_data;
        scene_data.reserve(
            sizeof(m_EntitySize)
          + free_entities.size_bytes()
          + entities.size_bytes()
          + component_pools.size_bytes()
        );

        LOG_ENGINE_INFO("Size: {}", m_EntitySize);

        auto push_to_scene_data_as_bytes = [&scene_data](auto number) -> bool
        {
            static_assert(
                std::is_trivially_constructible_v<decltype(number)>, 
                "The passed argument is not trivially constructible, therefore cannot be converted to a byte array!"
            );
            std::size_t i{sizeof(number) * 8};
            std::array<byte, sizeof(number)> byte_array;
            while (i != 0)
            {
                byte_array[(i / 8) - 1] = static_cast<byte>(number >> i);
                i -= 8;
            }

            std::copy(byte_array.begin(), byte_array.end(), std::back_inserter(scene_data));

            return true;
        };

        // Push the entity size to scene_data
        push_to_scene_data_as_bytes(m_EntitySize);

        // Push the free_entities array to scene_data
        push_to_scene_data_as_bytes(free_entities.size());
        for (auto const& entity_index : free_entities)
        {
            push_to_scene_data_as_bytes(entity_index);
        }

        // Push entities array to scene_data
        for (auto const& [entity_id, comp_mask] : entities)
        {
            push_to_scene_data_as_bytes(entity_id);
            push_to_scene_data_as_bytes(comp_mask.to_ullong());
        }

        // Push component pools to to scene_data
        for (auto& pool : component_pools)
        {
            auto byte_array = pool.Serialize();
            std::copy(byte_array.begin(), byte_array.end(), std::back_inserter(scene_data));
        }

        return scene_data;
    }
} // namespace polos
