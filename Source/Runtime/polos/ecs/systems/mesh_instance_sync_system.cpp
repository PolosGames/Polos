///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/systems/mesh_instance_sync_system.hpp"

#include "polos/ecs/components/mesh_renderer_component.hpp"
#include "polos/rendering/cache/material_cache.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/scene/scene.hpp"
#include "polos/utils/profiling.hpp"

namespace polos::ecs
{

namespace
{
constexpr auto kTransformBit    = static_cast<std::size_t>(ComponentType::kTransform);
constexpr auto kMeshRendererBit = static_cast<std::size_t>(ComponentType::kMeshRenderer);
}// namespace

void MeshInstanceSyncSystem::Update(scene::Scene* tScene)
{
    POLOS_PROFILE_ZONE("Sync mesh instances");
    if (tScene == nullptr)
    {
        return;
    }

    for (auto const handle : tScene->mRenderData.meshInstanceChanges)
    {
        if (tScene->isEntityValid(handle))
        {
            updateInstance(*tScene, handle);
        }
        else if (!IsEntityValid(tScene->GetEntityAtIndex(handle.index)))
        {
            // A replacement entity has its own creation record for this slot.
            rendering::MeshIndex const oldMeshId = tScene->mRenderData.meshInstances[handle.index].meshId;
            if (oldMeshId != rendering::kInvalidMeshId)
            {
                rendering::MeshCache::Get().DecreaseMeshUseByIndex(oldMeshId);
            }
            tScene->mRenderData.meshInstances[handle.index] = {};
            tScene->mRenderData.pendingMeshUploads.insert(handle.index);
        }
    }

    tScene->mRenderData.meshInstanceChanges.clear();

    for (auto const& dirty : tScene->GetDirtyEntities())
    {
        if ((dirty.componentMask.test(kTransformBit) || dirty.componentMask.test(kMeshRendererBit)))
        {
            updateInstance(*tScene, dirty.handle);
        }
    }
}

void MeshInstanceSyncSystem::updateInstance(scene::Scene& tScene, EntityHandle tEntity)
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
        instance.scale    = transform.scale;
    }
    if (mask.test(kMeshRendererBit))
    {
        auto* const mesh = tScene.getComponent<MeshRendererComponent>(tEntity.index);
        instance.meshId  = rendering::MeshCache::Get().GetMeshIndex(mesh->meshId);

        auto const& materials = rendering::MaterialCache::Get();
        auto const* material  = materials.GetMaterial(mesh->materialId);
        if (material == nullptr)
        {
            material = materials.GetMaterial(rendering::kDefaultMaterial);
        }

        instance.texID        = rendering::TextureCache::Get().GetTextureIndex(material->baseColorTexture);
        instance.tilingFactor = material->tilingFactor;
    }

    rendering::MeshIndex const oldMeshId = tScene.mRenderData.meshInstances[tEntity.index].meshId;
    if (oldMeshId != instance.meshId)
    {
        if (oldMeshId != rendering::kInvalidMeshId)
        {
            rendering::MeshCache::Get().DecreaseMeshUseByIndex(oldMeshId);
        }
        if (instance.meshId != rendering::kInvalidMeshId)
        {
            rendering::MeshCache::Get().IncreaseMeshUseByIndex(instance.meshId);
        }
    }

    tScene.mRenderData.meshInstances[tEntity.index] = instance;
    tScene.mRenderData.pendingMeshUploads.insert(tEntity.index);
}

}// namespace polos::ecs
