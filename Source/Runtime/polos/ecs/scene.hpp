
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_ECS_SCENE_HPP
#define POLOS_ECS_SCENE_HPP

#include "polos/ecs/component_pool.hpp"
#include "polos/ecs/entity.hpp"
#include "polos/ecs/scene_graph.hpp"
#include "polos/ecs/settings.hpp"
#include "polos/polos_api.hpp"
#include "polos/utils/string_id.hpp"

#include <glm/glm.hpp>

#include <array>
#include <vector>

namespace polos::ecs
{

class POLOS_API Scene
{
public:
    explicit Scene(utils::string_id tName = "Scene"_sid);
    ~Scene();

    Scene(Scene const&)            = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&)      = delete;

    /// @brief Create new entity inside entity pool
    /// @param tParent Parent of the entity to assign to, defaults to sceneRoot
    /// @param tName Initial name of entity, in string_id
    auto NewEntity(
        EntityHandle     tParent = EntityHandle{.index = 0U, .version = 0U},
        utils::string_id tName   = "New Entity"_sid) -> EntityHandle;

    /// Retires the entity and its descendants. The scene root cannot be retired.
    auto RetireEntity(EntityHandle tHandle) -> bool;

    [[nodiscard]] auto GetParent(EntityHandle tChild) -> EntityHandle;
    /// Returns the first child; iterate the others with GetNextSibling.
    [[nodiscard]] auto GetChild(EntityHandle tParent) -> EntityHandle;
    [[nodiscard]] auto GetNextSibling(EntityHandle tChild) -> EntityHandle;

    template<typename T>
    auto AddComponent(EntityHandle const tHandle, T const& tComponent) -> T*
    {
        if (!isEntityValid(tHandle))
        {
            LogError("Entity is not valid.");
            return nullptr;
        }

        ComponentType type = T::kType;
        if (HasComponent(tHandle, type))
        {
            LogError("Entity already has the requested component {}.", static_cast<std::uint32_t>(type));
            return nullptr;
        }

        return addComponent(tHandle.index, tComponent);
    }

    template<typename T>
    auto GetComponent(EntityHandle const tHandle) -> T*
    {
        if (!isEntityValid(tHandle))
        {
            LogError("Entity is not valid.");
            return nullptr;
        }

        ComponentType type = T::kType;
        if (!HasComponent(tHandle, type))
        {
            LogError("Entity do not have the requested component {}.", static_cast<std::uint32_t>(type));
            return nullptr;
        }

        return getComponent<T>(tHandle.index);
    }

    auto HasComponent(EntityHandle tHandle, ComponentType tCompType) -> bool;
    auto RemoveComponent(EntityHandle tHandle, ComponentType tCompType) -> bool;

    [[nodiscard]] auto GetEntityAtIndex(EntityIndex tIndex) const -> EntityHandle;

    /// @brief Number of allocated entity slots, including retired slots
    [[nodiscard]] auto GetEntityPoolSize() const -> EntityIndex;

    /// @brief Returns nullptr for indices outside the allocated entity slots.
    [[nodiscard]] auto GetEntityDataByIndex(EntityIndex tIndex) const -> EntityData const*;

    auto SetActiveCamera(EntityHandle tHandle) -> bool;

    utils::string_id name{0};

    EntityHandle activeCamera{kInvalidEntityHandle};
    EntityHandle sceneRoot{kInvalidEntityHandle};
private:
    friend class SceneManager;

    auto newEntity() -> EntityHandle;
    auto isEntityValid(EntityHandle tHandle) -> bool;

    template<typename T>
    auto addComponent(EntityIndex const tIndex, T const& tComponent) -> T*
    {
        Entity&        entity = mEntities[tIndex];
        ComponentType  type   = T::kType;
        ComponentPool& pool   = mComponentPools[static_cast<std::size_t>(type)];

        if (!pool.IsInitialized())
        {
            pool.Create<T>();
        }

        auto* component = pool.Get<T>(tIndex);
        *component      = tComponent;
        ++pool.mTotalComponents;

        entity.componentMask.set(static_cast<std::size_t>(type));

        return component;
    }

    template<typename T>
    auto getComponent(EntityIndex const tIndex) -> T*
    {
        return mComponentPools[static_cast<std::size_t>(T::kType)].Get<T>(tIndex);
    }

    std::array<Entity, Settings::kMaxEntities> mEntities;
    std::array<ComponentPool, kMaxComponents>  mComponentPools;

    std::uint32_t              mTotalEntities{0U};
    std::uint32_t              mEntitiesHead{0U};
    std::vector<std::uint32_t> mFreeIndices;

    SceneGraph mSceneGraph;
};

}// namespace polos::ecs

#endif// POLOS_ECS_SCENE_HPP
