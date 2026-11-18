///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///
#include "helpers.hpp"

#include "polos/ecs/components/info_component.hpp"

#include <polos/ecs/components/mesh_renderer_component.hpp>
#include <polos/ecs/components/transform_component.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/rendering/cache/material_cache.hpp>
#include <polos/rendering/cache/mesh_cache.hpp>
#include <polos/resource/resource_manager.hpp>

namespace ctc
{

using namespace polos;
auto CreateEntityFromModel(utils::string_id const tModel, ecs::Scene* tScene) -> ecs::EntityHandle
{
    auto parent = tScene->NewEntity();
    tScene->AddComponent(parent, ecs::TransformComponent{});
    tScene->GetMutableComponent<ecs::InfoComponent>(parent)->debugName = tModel;

    auto model = resource::ResourceManager::Get().GetModel(tModel);
    for (auto& mesh : model->meshes)
    {
        auto meshEntity = tScene->NewEntity(parent);
        tScene->AddComponent(meshEntity, ecs::TransformComponent{});
        tScene->GetMutableComponent<ecs::InfoComponent>(meshEntity)->debugName = utils::StrHash64Runtime(mesh.name);
        for (auto& primitive : mesh.primitives)
        {
            auto primitiveEntity = tScene->NewEntity(meshEntity);
            tScene->AddComponent(primitiveEntity, ecs::TransformComponent{});
            tScene->AddComponent(
                primitiveEntity, ecs::MeshRendererComponent{
                                     .meshHandle = rendering::MeshCache::Get().AddMesh(primitive),
                                     .matHandle  = primitive.material,
                                 });
            tScene->GetMutableComponent<ecs::InfoComponent>(primitiveEntity)->debugName =
                utils::StrHash64Runtime(primitive.name);
        }
    }

    return parent;
}

auto CreateLightSource(ecs::Scene* tScene, ecs::LightComponent const& tLight) -> ecs::EntityHandle
{
    auto const name   = utils::StrHash64Runtime("Light Source");
    auto const entity = tScene->NewEntity(tScene->sceneRoot, name);
    tScene->AddComponent(entity, tLight);
    return entity;
}

void CreateQuad(
    ecs::Scene*                     tScene,
    rendering::MaterialHandle const tMaterial,
    ecs::TransformComponent const&  tTransform)
{
    using namespace polos;

    auto quad = tScene->NewEntity();
    tScene->AddComponent(quad, tTransform);

    tScene->AddComponent(
        quad, ecs::MeshRendererComponent{
                  .meshHandle = rendering::kDefaultQuad,
                  .matHandle  = tMaterial,
              });
}

auto CreateMaterialFromTexture(std::string_view tName, resource::material::MaterialTexture const& tTexture)
    -> rendering::MaterialHandle
{
    auto const                   nameHash = utils::StrHash64Runtime(tName);
    resource::material::Material resMat{
        .name             = nameHash,
        .baseColorFactor  = {1.0F, 1.0F, 1.0F, 1.0F},
        .baseColorTexture = tTexture,
    };

    return rendering::MaterialCache::Get().AddMaterial(nameHash, rendering::Material::Create(resMat));
}

}// namespace ctc
