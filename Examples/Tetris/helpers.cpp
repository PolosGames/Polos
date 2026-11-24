///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///
#include "helpers.hpp"

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/info_component.hpp"

#include <polos/ecs/components/mesh_renderer_component.hpp>
#include <polos/ecs/components/transform_component.hpp>
#include <polos/rendering/cache/material_cache.hpp>
#include <polos/rendering/cache/mesh_cache.hpp>
#include <polos/resource/resource_manager.hpp>
#include <polos/scene/scene.hpp>
#include <polos/utils/pair.hpp>

#include <vector>

namespace ctc
{

using namespace polos;
auto CreateEntityFromModel(scene::Scene* tScene, utils::string_id tModel, ecs::TransformComponent const& tTransform, std::string_view tName)
    -> ecs::EntityHandle
{
    auto parent = tScene->NewEntity();
    tScene->AddComponent(parent, tTransform);
    tScene->GetMutableComponent<ecs::InfoComponent>(parent)->debugName = tName.empty() ? tModel : utils::StrHash64Runtime(tName);

    auto                                                     model = resource::ResourceManager::Get().GetModel(tModel);
    std::vector<utils::Pair<std::size_t, ecs::EntityHandle>> pending;
    pending.reserve(model->nodes.size());

    // Stack order attaches siblings in reverse because NewEntity prepends them.
    for (auto const root : model->rootNodes)
    {
        pending.push_back({root, parent});
    }

    while (!pending.empty())
    {
        auto const [index, nodeParent] = pending.back();
        pending.pop_back();
        auto const& node       = model->nodes[index];
        auto const  name       = node.name.empty() ? "Node." + std::to_string(index) : node.name;
        auto        nodeEntity = tScene->NewEntity(nodeParent, utils::StrHash64Runtime(name));
        tScene->AddComponent(
            nodeEntity, ecs::TransformComponent{
                            .position = node.position,
                            .rotation = glm::degrees(glm::eulerAngles(node.rotation)),
                            .scale    = node.scale,
                        });

        if (node.meshIndex)
        {
            auto& mesh = model->meshes[*node.meshIndex];
            for (auto& primitive : mesh.primitives)
            {
                auto primitiveEntity = tScene->NewEntity(nodeEntity, utils::StrHash64Runtime(primitive.name));
                tScene->AddComponent(primitiveEntity, ecs::TransformComponent{});
                tScene->AddComponent(
                    primitiveEntity, ecs::MeshRendererComponent{
                                         .meshHandle = rendering::MeshCache::Get().AddMesh(primitive),
                                         .matHandle  = primitive.material,
                                     });
            }
        }

        for (auto const child : node.children)
        {
            pending.push_back({child, nodeEntity});
        }
    }

    return parent;
}

auto CreateLightSource(scene::Scene* tScene, ecs::LightComponent const& tLight, std::string_view tName) -> ecs::EntityHandle
{
    auto const name   = utils::StrHash64Runtime("Light Source");
    auto const entity = tScene->NewEntity(tScene->sceneRoot, name);

    tScene->GetMutableComponent<ecs::InfoComponent>(entity)->debugName = utils::StrHash64Runtime(tName);
    tScene->AddComponent(entity, tLight);
    return entity;
}

auto CreateQuad(scene::Scene* tScene, rendering::MaterialHandle const tMaterial, ecs::TransformComponent const& tTransform, std::string_view tName)
    -> ecs::EntityHandle
{
    using namespace polos;

    auto quad = tScene->NewEntity();

    tScene->GetMutableComponent<ecs::InfoComponent>(quad)->debugName = utils::StrHash64Runtime(tName);
    tScene->AddComponent(quad, tTransform);
    tScene->AddComponent(
        quad, ecs::MeshRendererComponent{
                  .meshHandle = rendering::kDefaultQuad,
                  .matHandle  = tMaterial,
              });
    return quad;
}

void CreateGrid(scene::Scene* tScene, rendering::MaterialHandle tMaterial, glm::ivec2 tSize, glm::vec3 tPosition)
{
    resource::model::MeshPrimitive grid{
        .name              = "Tetris.Grid",
        .primitiveVertices = {},
        .primitiveIndices  = {0U, 1U, 2U, 2U, 3U, 0U},
        .material          = tMaterial,
        .bounds            = {},
    };
    grid.primitiveVertices.reserve(4U);
    glm::vec2 const size{tSize};
    for (auto const corner : {glm::vec2{0.0F, 0.0F}, glm::vec2{1.0F, 0.0F}, glm::vec2{1.0F, 1.0F}, glm::vec2{0.0F, 1.0F}})
    {
        grid.primitiveVertices.push_back({
            .position = glm::vec3{(corner - 0.5F) * size, 0.0F},
            .normal   = {0.0F, 0.0F, 1.0F},
            .tangent  = {1.0F, 0.0F, 0.0F, 1.0F},
            .texCoord = corner * size,
        });
    }

    auto const entity = tScene->NewEntity(tScene->sceneRoot, "Tetris Grid"_sid);
    tScene->AddComponent(entity, ecs::TransformComponent{.position = tPosition});
    tScene->AddComponent(entity, ecs::MeshRendererComponent{.meshHandle = rendering::MeshCache::Get().AddMesh(grid), .matHandle = tMaterial});
}

auto CreateMaterialFromTexture(std::string_view tTexture, std::string_view tName, rendering::SamplerDetails const& tSamplerDetails, float tTilingFactor)
    -> rendering::MaterialHandle
{
    auto const matTexture = resource::material::MaterialTexture{
        .img            = utils::StrHash64Runtime(tTexture),
        .textureDetails = tSamplerDetails,
    };

    auto const                   nameHash = tName.empty() ? matTexture.img : utils::StrHash64Runtime(tName);
    resource::material::Material resMat{
        .name             = nameHash,
        .baseColorFactor  = {1.0F, 1.0F, 1.0F, 1.0F},
        .baseColorTexture = matTexture,
        .tilingFactor     = tTilingFactor,
    };

    return rendering::MaterialCache::Get().AddMaterial(nameHash, rendering::Material::Create(resMat));
}

auto CreateCamera(scene::Scene* tScene, ecs::TransformComponent const& tTransform, ecs::CameraType tType, std::string_view tName, ecs::EntityHandle tParent)
    -> ecs::EntityHandle
{
    auto camera = tScene->NewEntity(tParent);

    tScene->GetMutableComponent<ecs::InfoComponent>(camera)->debugName = utils::StrHash64Runtime(tName);
    tScene->AddComponent(camera, tTransform);
    tScene->AddComponent(camera, ecs::CameraComponent{.cameraType = tType, .nearZ = 0.1F, .farZ = 800.0F, .fov = 60.0F});

    return camera;
}

auto CreateCube(
    scene::Scene*                  tScene,
    ecs::TransformComponent const& tTransform,
    rendering::MaterialHandle      tMaterial,
    std::string_view               tName,
    ecs::EntityHandle              tParent) -> ecs::EntityHandle
{
    auto cube = tScene->NewEntity(tParent);

    tScene->GetMutableComponent<ecs::InfoComponent>(cube)->debugName = utils::StrHash64Runtime(tName);
    tScene->AddComponent(cube, tTransform);
    tScene->AddComponent(cube, ecs::MeshRendererComponent{.meshHandle = rendering::kDefaultCube, .matHandle = tMaterial});

    return cube;
}

}// namespace ctc
