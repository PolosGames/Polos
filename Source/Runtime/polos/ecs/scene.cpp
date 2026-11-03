//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/ecs/scene.hpp"

#include "polos/ecs/components/info_component.hpp"
#include "polos/logging/log_macros.hpp"

#include <glm/gtc/quaternion.hpp>

#include <cassert>

namespace polos::ecs
{

Scene::Scene(utils::string_id const tName)
    : name(tName)
{
    sceneRoot = newEntity();

    addComponent(
        sceneRoot.index,
        InfoComponent{
            .debugName   = "SceneRoot"_sid,
            .graphHandle = SceneGraphHandle{0U},
        });
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
        child.index,
        InfoComponent{
            .debugName   = tName,
            .graphHandle = graphHandle,
        });

    ++parentInfo->childCount;

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

    mEntities[tHandle.index].componentMask[static_cast<std::size_t>(tCompType)].flip();
    --mComponentPools[static_cast<std::size_t>(tCompType)].mTotalComponents;

    return true;
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
