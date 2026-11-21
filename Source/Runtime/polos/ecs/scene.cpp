//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/ecs/scene.hpp"

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/logging/log_macros.hpp"

#include <glm/gtc/quaternion.hpp>

#include <cassert>

namespace polos::ecs
{

Scene::Scene(utils::string_id const tName)
    : Scene(tName, true)
{}

Scene::Scene(utils::string_id const tName, bool const tCreateEditorCamera)
    : name(tName)
{
    mRenderData.meshInstances.resize(Settings::kMaxEntities);

    sceneRoot = newEntity();

    addComponent(
        sceneRoot.index, InfoComponent{
                             .debugName   = "SceneRoot"_sid,
                             .graphHandle = SceneGraphHandle{0U},
                             .isTreeOpen  = true,
                         });

    if (tCreateEditorCamera)
    {
        auto const camera = NewEntity(sceneRoot, "Editor Camera"_sid);
        AddComponent(camera, TransformComponent{.position = {0.0F, 0.0F, 5.0F}});
        AddComponent(camera, CameraComponent{.nearZ = 0.1F, .farZ = 10000.0F});
        AddComponent(camera, EditorCameraControllerComponent{});
        SetActiveCamera(camera);
    }
}

Scene::~Scene() = default;

auto Scene::NewEntity(EntityHandle tParent, utils::string_id const tName) -> EntityHandle
{
    if (!isEntityValid(tParent))
    {
        LogError("Entity is not valid.");
        return kInvalidEntityHandle;
    }

    auto*      parentInfo     = getComponent<InfoComponent>(tParent.index);
    auto const parentPosGraph = parentInfo->graphHandle;

    auto child = newEntity();
    if (!IsEntityValid(child))
    {
        return kInvalidEntityHandle;
    }
    auto const graphHandle = mSceneGraph.AddChild(parentPosGraph, child);
    if (graphHandle == kInvalidSceneGraphHandle)
    {
        auto& entity        = mEntities[child.index];
        entity.handle.index = kInvalidEntityIndex;
        mFreeIndices.push_back(child.index);
        --mTotalEntities;
        return kInvalidEntityHandle;
    }
    addComponent(
        child.index, InfoComponent{
                         .debugName   = tName,
                         .graphHandle = graphHandle,
                     });

    ++parentInfo->childCount;
    mRenderData.meshInstanceChanges.push_back(child);

    return child;
}

auto Scene::RetireEntity(EntityHandle const tHandle) -> bool
{
    if (!isEntityValid(tHandle))
    {
        LogError("Entity is not valid.");
        return false;
    }

    if (tHandle.index == sceneRoot.index)
    {
        return false;
    }

    auto const graphHandle = getComponent<InfoComponent>(tHandle.index)->graphHandle;
    auto const root        = static_cast<SceneGraph::Index>(graphHandle);
    auto       current     = root;
    while (true)
    {
        auto const& node   = mSceneGraph.mNodes[current];
        auto const  handle = node.entity;
        auto&       entity = mEntities[handle.index];

        mRenderData.meshInstanceChanges.push_back(handle);
        discardDirtyEntity(handle.index);
        entity.handle.index = kInvalidEntityIndex;
        for (std::size_t i{0}; i < kMaxComponents; ++i)
        {
            if (entity.componentMask.test(i))
            {
                --mComponentPools[i].mTotalComponents;
            }
        }
        entity.componentMask.reset();

        ++entity.handle.version;

        if (entity.handle.version < kInvalidEntityVersion)
        {
            mFreeIndices.push_back(handle.index);
        }
        if (activeCamera.index == handle.index && activeCamera.version == handle.version)
        {
            activeCamera = kInvalidEntityHandle;
        }
        --mTotalEntities;

        if (node.leftChild != SceneGraph::kInvalidIndex)
        {
            current = node.leftChild;
            continue;
        }
        while (current != root && mSceneGraph.mNodes[current].rightSibling == SceneGraph::kInvalidIndex)
        {
            current = mSceneGraph.mNodes[current].parent;
        }
        if (current == root)
        {
            break;
        }
        current = mSceneGraph.mNodes[current].rightSibling;
    }

    --getComponent<InfoComponent>(mSceneGraph.GetParent(graphHandle).index)->childCount;
    mSceneGraph.RetireNode(graphHandle);

    return true;
}

auto Scene::GetParent(EntityHandle const tChild) -> EntityHandle
{
    return isEntityValid(tChild) ? mSceneGraph.GetParent(getComponent<InfoComponent>(tChild.index)->graphHandle)
                                 : kInvalidEntityHandle;
}

auto Scene::GetChild(EntityHandle const tParent) -> EntityHandle
{
    return isEntityValid(tParent) ? mSceneGraph.GetChild(getComponent<InfoComponent>(tParent.index)->graphHandle)
                                  : kInvalidEntityHandle;
}

auto Scene::GetNextSibling(EntityHandle const tChild) -> EntityHandle
{
    return isEntityValid(tChild) ? mSceneGraph.GetNextSibling(getComponent<InfoComponent>(tChild.index)->graphHandle)
                                 : kInvalidEntityHandle;
}

auto Scene::HasComponent(EntityHandle const tHandle, ComponentType tCompType) -> bool
{
    if (!isEntityValid(tHandle))
    {
        LogError("Entity is not valid.");
        return false;
    }

    return mEntities[tHandle.index].componentMask[static_cast<std::size_t>(tCompType)];
}

auto Scene::RemoveComponent(EntityHandle const tHandle, ComponentType tCompType) -> bool
{
    if (!isEntityValid(tHandle))
    {
        LogError("Entity is not valid.");
        return false;
    }

    if (tCompType == ComponentType::kInfo)
    {
        return false;
    }

    if (!HasComponent(tHandle, tCompType))
    {
        LogError("Entity do not have the requested component {}.", static_cast<std::uint32_t>(tCompType));
        return false;
    }

    auto const componentIndex = static_cast<std::size_t>(tCompType);
    mEntities[tHandle.index].componentMask.reset(componentIndex);
    --mComponentPools[componentIndex].mTotalComponents;
    if (tCompType == ComponentType::kTransform || tCompType == ComponentType::kMeshRenderer)
    {
        mRenderData.meshInstanceChanges.push_back(tHandle);
    }

    auto const dirtyIndex = mDirtyIndexByEntity[tHandle.index];
    if (dirtyIndex != kNoDirtyEntry && mDirtyEntities[dirtyIndex].componentMask.reset(componentIndex).none())
    {
        discardDirtyEntity(tHandle.index);
    }

    return true;
}

auto Scene::GetDirtyEntities() const -> std::span<DirtyEntity const>
{
    return mDirtyEntities;
}

auto Scene::GetRenderData() -> SceneRenderData&
{
    return mRenderData;
}

auto Scene::GetRenderData() const -> SceneRenderData const&
{
    return mRenderData;
}


auto Scene::ClearDirtyEntities() -> void
{
    for (auto const& dirty : mDirtyEntities)
    {
        mDirtyIndexByEntity[dirty.handle.index] = kNoDirtyEntry;
    }
    mDirtyEntities.clear();
}

auto Scene::markDirty(EntityHandle tHandle, ComponentType tType) -> void
{
    auto& dirtyIndex = mDirtyIndexByEntity[tHandle.index];
    if (dirtyIndex == kNoDirtyEntry)
    {
        mDirtyEntities.push_back({.handle = tHandle, .componentMask = {}});
        dirtyIndex = mDirtyEntities.size() - 1U;
    }
    mDirtyEntities[dirtyIndex].componentMask.set(static_cast<std::size_t>(tType));
}

auto Scene::discardDirtyEntity(EntityIndex tIndex) -> void
{
    auto& dirtyIndex = mDirtyIndexByEntity[tIndex];
    if (dirtyIndex == kNoDirtyEntry)
    {
        return;
    }

    mDirtyEntities[dirtyIndex]                                   = mDirtyEntities.back();
    mDirtyIndexByEntity[mDirtyEntities[dirtyIndex].handle.index] = dirtyIndex;
    mDirtyEntities.pop_back();
    dirtyIndex = kNoDirtyEntry;
}

auto Scene::isEntityDirty(EntityIndex tIndex) -> bool
{
    return mDirtyIndexByEntity[tIndex] != kNoDirtyEntry;
}

auto Scene::GetEntityAtIndex(EntityIndex const tIndex) const -> EntityHandle
{
    auto const* entity = GetEntityDataByIndex(tIndex);
    return entity != nullptr ? entity->handle : kInvalidEntityHandle;
}

auto Scene::GetEntityPoolSize() const -> EntityIndex
{
    return mEntitiesHead;
}

auto Scene::GetEntityDataByIndex(EntityIndex const tIndex) const -> EntityData const*
{
    return tIndex < mEntitiesHead ? &mEntities[tIndex] : nullptr;
}

auto Scene::GetObjectsByLayer() -> std::vector<uint32_t> const&
{
    return mSceneGraph.mObjectCountByLayer;
}

auto Scene::SetActiveCamera(EntityHandle tHandle) -> bool
{
    if (!isEntityValid(tHandle))
    {
        LogError("Camera is not a valid entity.");
        return false;
    }

    if (!HasComponent(tHandle, ComponentType::kCamera))
    {
        LogError("Trying to set the active camera, but provided entity does not have a camera component");
        return false;
    }

    activeCamera = tHandle;

    return true;
}

auto Scene::newEntity() -> EntityHandle
{
    if (mEntitiesHead >= Settings::kMaxEntities && mFreeIndices.empty())
    {
        LogError("AT MAX ENTITIES! Cannot add more.");
        return kInvalidEntityHandle;
    }

    EntityIndex  index  = kInvalidEntityIndex;
    EntityHandle handle = kInvalidEntityHandle;
    if (mFreeIndices.empty())
    {
        mDirtyIndexByEntity.push_back(kNoDirtyEntry);
        index  = mEntitiesHead++;
        handle = EntityHandle{.index = index, .version = 0U};

        mEntities[index].handle = handle;
        mEntities[index].componentMask.reset();
    }
    else
    {
        index = mFreeIndices.back();
        mFreeIndices.pop_back();

        handle                  = EntityHandle{.index = index, .version = mEntities[index].handle.version};
        mEntities[index].handle = handle;
        mEntities[index].componentMask.reset();
    }

    ++mTotalEntities;

    return handle;
}

auto Scene::isEntityValid(EntityHandle const tHandle) -> bool
{
    return IsEntityValid(tHandle) && tHandle.index < mEntitiesHead &&
           mEntities[tHandle.index].handle.index == tHandle.index &&
           mEntities[tHandle.index].handle.version == tHandle.version;
}

}// namespace polos::ecs
