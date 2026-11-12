///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///
#include "helpers.hpp"

#include "polos/utils/string_map.hpp"

#include <polos/ecs/components/mesh_renderer_component.hpp>
#include <polos/ecs/components/transform_component.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/rendering/cache/mesh_cache.hpp>
#include <polos/rendering/cache/texture_cache.hpp>
#include <polos/resource/resource_manager.hpp>

namespace sandbox
{
auto CreateEntityFromModel(polos::utils::string_id const tModel, polos::ecs::Scene* tScene) -> polos::ecs::EntityHandle
{
    using namespace polos;

    auto parent = tScene->NewEntity();
    tScene->AddComponent(parent, ecs::TransformComponent{});

    auto model = resource::ResourceManager::Get().GetModel(tModel);
    for (auto& mesh : model->meshes)
    {
        auto meshEntity = tScene->NewEntity(parent);
        tScene->AddComponent(meshEntity, ecs::TransformComponent{});
        for (auto& part : mesh.parts)
        {
            // LogDebug("Trying to create texture {}", utils::GetStringFromsID(part.material.baseColorTexture.resourceId));
            auto const handle = rendering::TextureCache::Get().GetOrCreateTextureIndex(
                part.material.baseColorTexture.resourceId,
                part.material.baseColorTexture.samplerDetails);
            auto partEntity = tScene->NewEntity(meshEntity);
            tScene->AddComponent(partEntity, ecs::TransformComponent{});
            tScene->AddComponent(
                partEntity,
                ecs::MeshRendererComponent{
                    .meshHandle       = rendering::MeshCache::Get().AddMesh(part),
                    .baseColorTexture = handle,
                });
        }
    }

    return parent;
}
}// namespace sandbox
