///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/systems/dirty_transform_system.hpp"

#include "polos/ecs/components/mesh_renderer_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/utils/profiling.hpp"

namespace polos::ecs
{

namespace
{
constexpr auto kTransformBit    = static_cast<std::size_t>(ComponentType::kTransform);
constexpr auto kMeshRendererBit = static_cast<std::size_t>(ComponentType::kMeshRenderer);
}// namespace

void DirtyTransformSystem::Update(Scene* tScene)
{
    POLOS_PROFILE_ZONE("Dirty transforms and mesh instances");
    if (tScene == nullptr)
    {
        return;
    }

    for (auto const handle : tScene->mInstanceChanges)
    {
        if (tScene->isEntityValid(handle))
        {
            updateInstance(*tScene, handle);
        }
        else if (!IsEntityValid(tScene->GetEntityAtIndex(handle.index)))
        {
            // A replacement entity has its own creation record for this slot.
            uint32_t const oldMeshId = tScene->mInstanceBuffer[handle.index].meshId;
            if (oldMeshId != rendering::kInvalidMeshId)
            {
                rendering::MeshCache::DecreaseMeshUseByIndex(oldMeshId);
            }
            tScene->mInstanceBuffer[handle.index] = {};
            tScene->mPendingInstanceUploads.insert(handle.index);
        }
    }

    tScene->mInstanceChanges.clear();

    for (auto const& dirty : tScene->GetDirtyEntities())
    {
        if ((dirty.componentMask.test(kTransformBit) || dirty.componentMask.test(kMeshRendererBit)))
        {
            updateInstance(*tScene, dirty.handle);
        }
    }
}

void DirtyTransformSystem::updateInstance(Scene& tScene, EntityHandle tEntity)
{
    auto const&             mask = tScene.mEntities[tEntity.index].componentMask;
    rendering::MeshInstance instance{
        .parentId = tScene.GetParent(tEntity).index,
    };
    if (mask.test(kTransformBit))
    {
        auto const&     transform = *tScene.getComponent<TransformComponent>(tEntity.index);
        glm::quat const rotation{glm::radians(transform.rotation)};
        instance.position = transform.position;
        instance.rotation = glm::vec4(rotation.x, rotation.y, rotation.z, rotation.w);
        instance.scale    = transform.scale.x;
    }
    if (mask.test(kMeshRendererBit))
    {
        auto const mesh = tScene.getComponent<MeshRendererComponent>(tEntity.index)->meshHandle;
        instance.meshId = rendering::MeshCache::GetMeshIndexByHandle(mesh);
    }

    uint32_t const oldMeshId = tScene.mInstanceBuffer[tEntity.index].meshId;
    if (oldMeshId != instance.meshId)
    {
        if (oldMeshId != rendering::kInvalidMeshId)
        {
            rendering::MeshCache::DecreaseMeshUseByIndex(oldMeshId);
        }
        if (instance.meshId != rendering::kInvalidMeshId)
        {
            rendering::MeshCache::IncreaseMeshUseByIndex(instance.meshId);
        }
    }

    tScene.mInstanceBuffer[tEntity.index] = instance;
    tScene.mPendingInstanceUploads.insert(tEntity.index);
}

}// namespace polos::ecs
