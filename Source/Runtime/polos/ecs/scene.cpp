//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/ecs/scene.hpp"

#include "polos/logging/log_macros.hpp"

#include <glm/gtc/quaternion.hpp>

#include <cassert>

namespace polos::ecs
{

Scene::Scene() = default;

Scene::~Scene() = default;

auto Scene::NewEntity() -> EntityHandle
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

auto Scene::RetireEntity(EntityHandle const tHandle) -> bool
{
    if (!isEntityValid(tHandle))
    {
        LogError("Entity is not valid.");
        return false;
    }

    Entity& destinationEntt = mEntities[tHandle.index];
    if (destinationEntt.handle.version != tHandle.version)
    {
        LogError("Provided entity and entity at the destination index do not match. No removal happens.");
        return false;
    }

    destinationEntt.handle.index = kInvalidEntityIndex;
    if (destinationEntt.handle.version + 1 < kInvalidEntityVersion)
    {
        ++destinationEntt.handle.version;
    }
    mFreeIndices.push_back(tHandle.index);

    --mTotalEntities;

    return true;
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

    if (!HasComponent(tHandle, tCompType))
    {
        LogError("Entity do not have the requested component {}.", static_cast<std::uint32_t>(tCompType));
        return false;
    }

    mEntities[tHandle.index].componentMask[static_cast<std::size_t>(tCompType)].flip();

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

auto Scene::isEntityValid(EntityHandle tHandle) -> bool
{
    return IsEntityValid(tHandle) && tHandle.index < mEntitiesHead &&
           mEntities[tHandle.index].handle.index == tHandle.index &&
           mEntities[tHandle.index].handle.version == tHandle.version;
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

}// namespace polos::ecs
