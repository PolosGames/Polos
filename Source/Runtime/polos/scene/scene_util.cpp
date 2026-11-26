///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/scene/scene_util.hpp"

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/light_component.hpp"
#include "polos/ecs/components/mesh_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"
#include "polos/rendering/cache/model_cache.hpp"
#include "polos/scene/scene.hpp"
#include "polos/scene/scene_manager.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_id.hpp"
#include "polos/utils/string_map.hpp"
#include "polos/utils/triplet.hpp"

#include <array>
#include <cstdint>
#include <format>
#include <map>

namespace polos::scene
{

namespace
{
// {Dimension, meshHandle}
std::map<utils::Pair<float const, float const>, resource::ResourceId>                 gQuadRegistry;
std::map<utils::Triplet<float const, float const, float const>, resource::ResourceId> gCuboidRegistry;
}// namespace

auto NewEntity(ecs::EntityHandle const tParent, utils::string_id const tName) -> ecs::EntityHandle
{
    return scene::SceneManager::Get().GetCurrentScene()->NewEntity(tParent, tName);
}

auto RetireEntity(ecs::EntityHandle const tParent) -> bool
{
    return scene::SceneManager::Get().GetCurrentScene()->RetireEntity(tParent);
}

auto Create_TexturedQuad(QuadDetails const& tDetails, ecs::EntityHandle const tParent) -> ecs::EntityHandle
{
    auto const nameHash = utils::RegisterString(tDetails.name);
    auto const quad     = NewEntity(tParent, nameHash);

    {
        auto* transform     = AddComponent_Transform(quad);
        transform->position = tDetails.position;
        transform->rotation = tDetails.rotation;
        transform->scale    = tDetails.scale;
    }

    auto const           pairKey    = utils::MakePair(tDetails.dimensions.x, tDetails.dimensions.y);
    bool                 quadCached = gQuadRegistry.contains(pairKey);
    resource::ResourceId meshId     = quadCached ? gQuadRegistry.at(pairKey) : 0;

    if (!quadCached)
    {
        static constexpr std::array corners{
            glm::vec2{0.0F, 0.0F},
            glm::vec2{1.0F, 0.0F},
            glm::vec2{1.0F, 1.0F},
            glm::vec2{0.0F, 1.0F},
        };
        rendering::MeshPrimitiveData quadMesh;
        quadMesh.name = tDetails.name;
        quadMesh.primitiveVertices.reserve(corners.size());
        for (auto const& texCoord : corners)
        {
            quadMesh.primitiveVertices.push_back(
                rendering::MeshVertex{
                    .position = glm::vec3((texCoord - glm::vec2(0.5F)) * tDetails.dimensions, 0.0F),
                    .normal   = {0.0F, 0.0F, 1.0F},
                    .tangent  = {1.0F, 0.0F, 0.0F, 1.0F},
                    .texCoord = texCoord * tDetails.tilingFactor,
                });
        }
        quadMesh.primitiveIndices = {0U, 1U, 2U, 2U, 3U, 0U};
        quadMesh.material         = tDetails.material;

        meshId = gQuadRegistry[pairKey] =
            rendering::MeshCache::Get().AddMesh(utils::RegisterString(std::format("mesh.Quad-{}-{}", tDetails.dimensions.x, tDetails.dimensions.y)), quadMesh);
    }

    {
        auto* meshRenderer       = AddComponent_MeshRenderer(quad);
        meshRenderer->meshId     = meshId;
        meshRenderer->materialId = tDetails.material;
    }

    return quad;
}

auto Create_TexturedCuboid(CuboidDetails const& tDetails, ecs::EntityHandle const tParent) -> ecs::EntityHandle
{
    auto const& dimensions = tDetails.dimensions;
    auto const  cuboid     = NewEntity(tParent, utils::RegisterString(tDetails.name));

    {
        auto* transform     = AddComponent_Transform(cuboid);
        transform->position = tDetails.position;
        transform->rotation = tDetails.rotation;
        transform->scale    = tDetails.scale;
    }

    auto const           key    = utils::MakeTriplet(dimensions.x, dimensions.y, dimensions.z);
    auto const           cached = gCuboidRegistry.find(key);
    auto&                meshes = rendering::MeshCache::Get();
    resource::ResourceId meshId{};

    if (cached != gCuboidRegistry.end() && meshes.HasMesh(cached->second))
    {
        meshId = cached->second;
    }
    else
    {
        static constexpr std::array corners{
            glm::vec3{-0.5F, -0.5F, 0.5F},
            glm::vec3{0.5F, -0.5F, 0.5F},
            glm::vec3{0.5F, 0.5F, 0.5F},
            glm::vec3{-0.5F, 0.5F, 0.5F},
            glm::vec3{-0.5F, -0.5F, -0.5F},
            glm::vec3{0.5F, -0.5F, -0.5F},
            glm::vec3{0.5F, 0.5F, -0.5F},
            glm::vec3{-0.5F, 0.5F, -0.5F},
        };
        static constexpr std::array faces{
            std::array{4U, 0U, 3U, 7U},
            std::array{4U, 5U, 1U, 0U},
            std::array{0U, 1U, 2U, 3U},
            std::array{3U, 2U, 6U, 7U},
            std::array{5U, 4U, 7U, 6U},
            std::array{1U, 5U, 6U, 2U},
        };
        static constexpr std::array texCoords{
            glm::vec2{0.0F, 0.0F},
            glm::vec2{1.0F, 0.0F},
            glm::vec2{1.0F, 1.0F},
            glm::vec2{0.0F, 1.0F},
        };
        static constexpr std::array indices{0U, 1U, 2U, 2U, 3U, 0U};

        rendering::MeshPrimitiveData cuboidMesh{
            .name     = std::format("mesh.Cuboid-{}-{}-{}", dimensions.x, dimensions.y, dimensions.z),
            .material = tDetails.material,
        };
        cuboidMesh.primitiveVertices.reserve(faces.size() * texCoords.size());
        cuboidMesh.primitiveIndices.reserve(faces.size() * indices.size());

        for (auto const& face : faces)
        {
            auto const  firstVertex = static_cast<std::uint32_t>(cuboidMesh.primitiveVertices.size());
            auto const& origin      = corners[face[0]];
            auto const  tangent     = corners[face[1]] - origin;
            auto const  normal      = glm::cross(tangent, corners[face[3]] - origin);

            for (std::size_t i{0U}; i < face.size(); ++i)
            {
                cuboidMesh.primitiveVertices.push_back({
                    .position = corners[face[i]] * dimensions,
                    .normal   = normal,
                    .tangent  = glm::vec4(tangent, 1.0F),
                    .texCoord = texCoords[i],
                });
            }

            for (auto const index : indices)
            {
                cuboidMesh.primitiveIndices.push_back(firstVertex + index);
            }
        }

        meshId = gCuboidRegistry[key] = meshes.AddMesh(utils::RegisterString(cuboidMesh.name), cuboidMesh);
    }

    {
        auto* meshRenderer       = AddComponent_MeshRenderer(cuboid);
        meshRenderer->meshId     = meshId;
        meshRenderer->materialId = tDetails.material;
    }

    return cuboid;
}

auto Create_LightSource(LightDetails const& tDetails, ecs::EntityHandle const tParent) -> ecs::EntityHandle
{
    auto const nameHash = utils::RegisterString(tDetails.name);
    auto const entity   = NewEntity(tParent, nameHash);

    GetComponent_Info(entity)->debugName = nameHash;

    {
        auto* transform     = AddComponent_Transform(entity);
        transform->position = tDetails.position;
        transform->rotation = tDetails.rotation;
        transform->scale    = tDetails.scale;
    }

    {
        auto* light           = AddComponent_Light(entity);
        light->color          = tDetails.color;
        light->ambientFactor  = tDetails.ambientFactor;
        light->specularFactor = tDetails.specularFactor;
    }

    return entity;
}

auto Create_Model(ModelDetails const& tDetails, ecs::EntityHandle const tParent) -> ecs::EntityHandle
{
    auto const nameHash = utils::RegisterString(tDetails.name);

    auto const* model = rendering::ModelCache::Get().GetModel(tDetails.modelID);
    PL_ASSERT(nullptr != model, "Invalid model id");

    auto const entity = NewEntity(tParent, nameHash);

    {
        auto* transform     = AddComponent_Transform(entity);
        transform->position = tDetails.position;
        transform->rotation = tDetails.rotation;
        transform->scale    = tDetails.scale;
    }

    std::vector<utils::Pair<std::size_t, ecs::EntityHandle>> pending;
    pending.reserve(model->nodes.size());

    // Stack order attaches siblings in reverse because NewEntity prepends them.
    for (auto const root : model->rootNodes)
    {
        pending.push_back({.first = root, .second = entity});
    }

    while (!pending.empty())
    {
        auto const [index, nodeParent] = pending.back();
        pending.pop_back();
        auto const& node       = model->nodes[index];
        auto const  name       = node.name.empty() ? "Node." + std::to_string(index) : node.name;
        auto        nodeEntity = NewEntity(nodeParent, utils::RegisterString(name));

        {
            auto* transform     = AddComponent_Transform(nodeEntity);
            transform->position = node.position;
            transform->rotation = glm::degrees(glm::eulerAngles(node.rotation));
            transform->scale    = node.scale;
        }

        if (node.meshIndex)
        {
            auto const& mesh = model->meshes[*node.meshIndex];
            for (auto const primitiveId : mesh.primitives)
            {
                auto const* primitive = rendering::MeshCache::Get().GetMesh(primitiveId);
                PL_ASSERT(nullptr != primitive, "MeshPrimitive not found in cache! Loadin");

                auto primitiveEntity = NewEntity(nodeEntity, primitive->name);
                AddComponent_Transform(primitiveEntity);
                {
                    auto* meshRenderer       = AddComponent_MeshRenderer(primitiveEntity);
                    meshRenderer->meshId     = primitiveId;
                    meshRenderer->materialId = primitive->material;
                }
            }
        }

        for (auto const child : node.children)
        {
            pending.push_back({.first = child, .second = nodeEntity});
        }
    }

    return entity;
}

auto Create_Camera(CameraDetails const& tDetails, ecs::EntityHandle const tParent) -> ecs::EntityHandle
{
    auto const nameHash = utils::RegisterString(tDetails.name);
    auto const entity   = NewEntity(tParent, nameHash);

    {
        auto* transform     = AddComponent_Transform(entity);
        transform->position = tDetails.position;
        transform->rotation = tDetails.rotation;
        transform->scale    = tDetails.scale;
    }
    {
        auto* camera       = AddComponent_Camera(entity);
        camera->cameraType = tDetails.cameraType;
        camera->nearZ      = tDetails.nearZ;
        camera->farZ       = tDetails.farZ;

        switch (tDetails.cameraType)
        {
            case ecs::CameraType::kPerspective: camera->fov = tDetails.fov; break;
            case ecs::CameraType::kOrtho: camera->size = tDetails.size; break;
        }
    }

    return entity;
}

auto AddComponent_Camera(ecs::EntityHandle const tEntity) -> ecs::CameraComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->AddComponent(tEntity, ecs::CameraComponent{});
}

auto GetComponent_Camera(ecs::EntityHandle const tEntity) -> ecs::CameraComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetMutableComponent<ecs::CameraComponent>(tEntity);
}

auto GetConstComponent_Camera(ecs::EntityHandle const tEntity) -> ecs::CameraComponent const*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetComponent<ecs::CameraComponent>(tEntity);
}

auto AddComponent_MeshRenderer(ecs::EntityHandle const tEntity) -> ecs::MeshRendererComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->AddComponent(tEntity, ecs::MeshRendererComponent{});
}

auto GetComponent_MeshRenderer(ecs::EntityHandle const tEntity) -> ecs::MeshRendererComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetMutableComponent<ecs::MeshRendererComponent>(tEntity);
}

auto GetConstComponent_MeshRenderer(ecs::EntityHandle const tEntity) -> ecs::MeshRendererComponent const*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetComponent<ecs::MeshRendererComponent>(tEntity);
}

auto AddComponent_Transform(ecs::EntityHandle const tEntity) -> ecs::TransformComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->AddComponent(tEntity, ecs::TransformComponent{});
}

auto GetComponent_Transform(ecs::EntityHandle const tEntity) -> ecs::TransformComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetMutableComponent<ecs::TransformComponent>(tEntity);
}

auto GetConstComponent_Transform(ecs::EntityHandle const tEntity) -> ecs::TransformComponent const*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetComponent<ecs::TransformComponent>(tEntity);
}

auto GetComponent_Info(ecs::EntityHandle const tEntity) -> ecs::InfoComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetMutableComponent<ecs::InfoComponent>(tEntity);
}

auto GetConstComponent_Info(ecs::EntityHandle const tEntity) -> ecs::InfoComponent const*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetComponent<ecs::InfoComponent>(tEntity);
}

auto AddComponent_Light(ecs::EntityHandle const tEntity) -> ecs::LightComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->AddComponent(tEntity, ecs::LightComponent{});
}

auto GetComponent_Light(ecs::EntityHandle const tEntity) -> ecs::LightComponent*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetMutableComponent<ecs::LightComponent>(tEntity);
}

auto GetConstComponent_Light(ecs::EntityHandle const tEntity) -> ecs::LightComponent const*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetComponent<ecs::LightComponent>(tEntity);
}

auto EntityAtIndex(ecs::EntityIndex const tIndex) -> ecs::EntityHandle
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetEntityAtIndex(tIndex);
}

auto EntityDataAtIndex(ecs::EntityIndex const tIndex) -> ecs::Entity const*
{
    auto* scene = SceneManager::Get().GetCurrentScene();
    return scene->GetEntityDataByIndex(tIndex);
}

}// namespace polos::scene
