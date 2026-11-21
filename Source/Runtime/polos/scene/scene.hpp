
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_SCENE_SCENE_HPP
#define POLOS_SCENE_SCENE_HPP

#include "polos/ecs/component_pool.hpp"
#include "polos/ecs/components/mesh_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/entity.hpp"
#include "polos/ecs/settings.hpp"
#include "polos/polos_api.hpp"
#include "polos/scene/scene_data.hpp"
#include "polos/scene/scene_graph.hpp"
#include "polos/utils/error_code.hpp"
#include "polos/utils/string_id.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <span>
#include <unordered_map>
#include <vector>

namespace polos::ecs
{
class LightCollectionSystem;
class MeshInstanceSyncSystem;
}// namespace polos::ecs

namespace polos::scene
{

struct SceneSnapshot;

class POLOS_API Scene
{
public:
    explicit Scene(utils::string_id tName = "Scene"_sid);
    explicit Scene(utils::string_id tName, bool tCreateEditorCamera);
    ~Scene();

    Scene(Scene const&)            = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&)      = delete;

    /// @brief Create new entity inside entity pool
    /// @param tParent Parent of the entity to assign to, defaults to sceneRoot
    /// @param tName Initial name of entity, in string_id
    [[nodiscard]] auto NewEntity(
        ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U},
        utils::string_id  tName   = "New Entity"_sid) -> ecs::EntityHandle;

    /// Retires the entity and its descendants. The scene root cannot be retired.
    auto RetireEntity(ecs::EntityHandle tHandle) -> bool;

    [[nodiscard]] auto GetParent(ecs::EntityHandle tChild) -> ecs::EntityHandle;
    /// Returns the first child; iterate the others with GetNextSibling.
    [[nodiscard]] auto GetChild(ecs::EntityHandle tParent) -> ecs::EntityHandle;
    [[nodiscard]] auto GetNextSibling(ecs::EntityHandle tChild) -> ecs::EntityHandle;

    template<typename T>
    auto AddComponent(ecs::EntityHandle const tHandle, T const& tComponent) -> T*
    {
        if (!isEntityValid(tHandle))
        {
            LogError("Entity is not valid.");
            return nullptr;
        }

        ecs::ComponentType type = T::kType;
        if (HasComponent(tHandle, type))
        {
            LogError("Entity already has the requested component {}.", static_cast<std::uint32_t>(type));
            return nullptr;
        }

        auto* component = addComponent(tHandle.index, tComponent);
        if constexpr (T::kType == ecs::ComponentType::kTransform || T::kType == ecs::ComponentType::kMeshRenderer)
        {
            if (component != nullptr)
            {
                mRenderData.meshInstanceChanges.push_back(tHandle);
            }
        }
        return component;
    }

    /// @brief Returns the component that belong to the entity in an immutable form.
    /// @note Use GetMutableComponent to change the component.
    template<typename T>
    auto GetComponent(ecs::EntityHandle const tHandle) -> T const*
    {
        if (!isEntityValid(tHandle))
        {
            LogError("Entity is not valid.");
            return nullptr;
        }

        ecs::ComponentType type = T::kType;
        if (!HasComponent(tHandle, type))
        {
            LogError("Entity do not have the requested component {}.", static_cast<std::uint32_t>(type));
            return nullptr;
        }

        return getComponent<T>(tHandle.index);
    }

    /// @brief Returns the component that belong to the entity in a mutable form, and marks it dirty
    template<typename T>
    auto GetMutableComponent(ecs::EntityHandle const tHandle) -> T*
    {
        if (!isEntityValid(tHandle))
        {
            LogError("Entity is not valid.");
            return nullptr;
        }

        ecs::ComponentType type = T::kType;
        if (!HasComponent(tHandle, type))
        {
            LogError("Entity do not have the requested component {}.", static_cast<std::uint32_t>(type));
            return nullptr;
        }

        markDirty(tHandle, type);
        return getComponent<T>(tHandle.index);
    }

    template<typename T>
    auto HasComponent(ecs::EntityHandle const tHandle)
    {
        return HasComponent(tHandle, T::kType);
    }

    [[nodiscard]] auto GetDirtyEntities() const -> std::span<ecs::DirtyEntity const>;
    auto               ClearDirtyEntities() -> void;

    [[nodiscard]] auto GetRenderData() -> SceneRenderData&;
    [[nodiscard]] auto GetRenderData() const -> SceneRenderData const&;

    auto HasComponent(ecs::EntityHandle tHandle, ecs::ComponentType tCompType) -> bool;
    auto RemoveComponent(ecs::EntityHandle tHandle, ecs::ComponentType tCompType) -> bool;

    [[nodiscard]] auto GetEntityAtIndex(ecs::EntityIndex tIndex) const -> ecs::EntityHandle;

    /// @brief Number of allocated entity slots, including retired slots
    [[nodiscard]] auto GetEntityPoolSize() const -> ecs::EntityIndex;

    /// @brief Returns nullptr for indices outside the allocated entity slots.
    [[nodiscard]] auto GetEntityDataByIndex(ecs::EntityIndex tIndex) const -> ecs::EntityData const*;

    [[nodiscard]] auto GetObjectsByLayer() -> std::vector<uint32_t> const&;

    /// Capture on the thread that owns the scene. The snapshot can be saved on a worker.
    [[nodiscard]] auto TakeSnapshot() -> Result<std::shared_ptr<SceneSnapshot const>>;

    auto SetActiveCamera(ecs::EntityHandle tHandle) -> bool;

    utils::string_id name{0};

    ecs::EntityHandle activeCamera{ecs::kInvalidEntityHandle};
    ecs::EntityHandle sceneRoot{ecs::kInvalidEntityHandle};
private:
    friend class SceneManager;

    friend class ecs::LightCollectionSystem;
    friend class ecs::MeshInstanceSyncSystem;

    auto newEntity() -> ecs::EntityHandle;
    auto isEntityValid(ecs::EntityHandle tHandle) -> bool;

    auto markDirty(ecs::EntityHandle tHandle, ecs::ComponentType tType) -> void;
    auto discardDirtyEntity(ecs::EntityIndex tIndex) -> void;
    auto isEntityDirty(ecs::EntityIndex tIndex) -> bool;

    template<typename T>
    auto addComponent(ecs::EntityIndex const tIndex, T const& tComponent) -> T*
    {
        ecs::ComponentType type = T::kType;
        if (type != ecs::ComponentType::kInfo && tIndex == 0)
        {
            LogError("Trying to add component to scene root is not allowed.");
            return nullptr;
        }

        ecs::Entity&        entity = mEntities[tIndex];
        ecs::ComponentPool& pool   = mComponentPools[static_cast<std::size_t>(type)];

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
    auto getComponent(ecs::EntityIndex const tIndex) -> T*
    {
        return mComponentPools[static_cast<std::size_t>(T::kType)].Get<T>(tIndex);
    }

    std::array<ecs::Entity, ecs::Settings::kMaxEntities> mEntities;
    std::array<ecs::ComponentPool, ecs::kMaxComponents>  mComponentPools;

    static constexpr auto    kNoDirtyEntry = std::numeric_limits<std::size_t>::max();
    std::vector<std::size_t> mDirtyIndexByEntity;
    std::vector<ecs::DirtyEntity> mDirtyEntities;

    std::uint32_t              mTotalEntities{0U};
    std::uint32_t              mEntitiesHead{0U};
    std::vector<std::uint32_t> mFreeIndices;

    SceneRenderData mRenderData;

    SceneGraph mSceneGraph;
};

}// namespace polos::scene

#endif// POLOS_SCENE_SCENE_HPP
