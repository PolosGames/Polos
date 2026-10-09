//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_ECS_SCENE_HPP
#define POLOS_ECS_SCENE_HPP

#include "polos/ecs/component_pool.hpp"
#include "polos/ecs/entity.hpp"
#include "polos/ecs/settings.hpp"
#include "polos/polos_api.hpp"

#include <glm/glm.hpp>

#include <array>
#include <vector>

namespace polos::ecs
{

class POLOS_API Scene
{
public:
    Scene();
    ~Scene();

    Scene(Scene const&)            = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&)      = delete;

    auto NewEntity() -> EntityHandle;
    auto RetireEntity(EntityHandle tHandle) -> bool;

    template<typename T>
    auto AddComponent(EntityHandle const tHandle, T const& tComponent) -> T*
    {
        if (!isEntityValid(tHandle))
        {
            LogError("Entity is not valid.");
            return nullptr;
        }

        Entity& entity = mEntities[tHandle.index];

        ComponentType type = T::kType;
        if (HasComponent(tHandle, type))
        {
            LogError("Entity already has the requested component {}.", static_cast<std::uint32_t>(type));
            return nullptr;
        }

        ComponentPool& pool = mComponentPools[static_cast<std::size_t>(type)];

        if (!pool.IsInitialized())
        {
            pool.Create<T>();
        }

        auto* component = pool.Get<T>(tHandle.index);
        *component      = tComponent;

        entity.componentMask.set(static_cast<std::size_t>(type));

        return component;
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

        return mComponentPools[static_cast<std::size_t>(type)].Get<T>(tHandle.index);
    }

    auto HasComponent(EntityHandle tHandle, ComponentType tCompType) -> bool;
    auto RemoveComponent(EntityHandle tHandle, ComponentType tCompType) -> bool;

    [[nodiscard]] auto GetEntityAtIndex(EntityIndex tIndex) const -> EntityHandle;

    /// @brief Number of allocated entity slots, including retired slots
    [[nodiscard]] auto GetEntityPoolSize() const -> EntityIndex;

    /// @brief Returns nullptr for indices outside the allocated entity slots.
    [[nodiscard]] auto GetEntityDataByIndex(EntityIndex tIndex) const -> EntityData const*;

    auto SetActiveCamera(EntityHandle tHandle) -> bool;

    EntityHandle activeCamera{kInvalidEntityHandle};
private:
    friend class SceneManager;

    auto isEntityValid(EntityHandle tHandle) -> bool;

    std::array<Entity, Settings::kMaxEntities>          mEntities;
    std::array<ComponentPool, Settings::kMaxComponents> mComponentPools;

    std::uint32_t              mTotalEntities{0U};
    std::uint32_t              mEntitiesHead{0U};
    std::vector<std::uint32_t> mFreeIndices;
};

}// namespace polos::ecs

#endif// POLOS_ECS_SCENE_HPP
