///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SCENE_DATA_HPP
#define POLOS_ECS_SCENE_DATA_HPP

#include "polos/ecs/entity.hpp"
#include "polos/rendering/push_constants/scene_light_info.hpp"
#include "polos/rendering/ssbo/light_instance.hpp"
#include "polos/rendering/ssbo/mesh_instance.hpp"

#include <array>
#include <unordered_set>
#include <vector>

namespace polos::ecs
{

struct SceneRenderData
{
    std::vector<rendering::MeshInstance> meshInstances;
    std::vector<EntityHandle>            meshInstanceChanges;
    std::unordered_set<EntityIndex>      pendingMeshUploads;

    rendering::SceneLightInfo                                  lightInfo;
    std::array<rendering::LightInstance, Settings::kMaxLights> lightInstances;
};

}// namespace polos::ecs

#endif// POLOS_ECS_SCENE_DATA_HPP
