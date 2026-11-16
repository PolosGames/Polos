///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///
#include "helpers.hpp"

#include <polos/ecs/components/mesh_renderer_component.hpp>
#include <polos/ecs/components/transform_component.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/rendering/cache/material_cache.hpp>
#include <polos/rendering/cache/mesh_cache.hpp>
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
            auto partEntity = tScene->NewEntity(meshEntity);
            tScene->AddComponent(partEntity, ecs::TransformComponent{});
            tScene->AddComponent(
                partEntity, ecs::MeshRendererComponent{
                                .meshHandle = rendering::MeshCache::Get().AddMesh(part),
                                .matHandle  = part.material,
                            });
        }
    }

    return parent;
}

auto CreateLightSource(polos::ecs::Scene* tScene, polos::ecs::LightComponent const& tLight) -> polos::ecs::EntityHandle
{
    auto const name   = polos::utils::StrHash64Runtime("Light Source");
    auto const entity = tScene->NewEntity(tScene->sceneRoot, name);
    tScene->AddComponent(entity, tLight);
    return entity;
}

void CreateQuad(
    polos::ecs::Scene*                     tScene,
    glm::vec3 const                        tPosition,
    glm::vec3 const                        tRotation,
    float const                            tScale,
    polos::rendering::MaterialHandle const tMaterial)
{
    using namespace polos;

    auto quad = tScene->NewEntity();
    tScene->AddComponent(
        quad, ecs::TransformComponent{
                  .position = tPosition,
                  .rotation = tRotation,
                  .scale    = tScale,
              });

    tScene->AddComponent(
        quad, ecs::MeshRendererComponent{
                  .meshHandle = rendering::kDefaultQuad,
                  .matHandle  = tMaterial,
              });
}

}// namespace sandbox
