///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_SCENE_SCENE_DATA_HPP
#define POLOS_SCENE_SCENE_DATA_HPP

#include "polos/ecs/entity.hpp"
#include "polos/rendering/push_constants/scene_light_info.hpp"
#include "polos/rendering/ssbo/light_instance.hpp"
#include "polos/rendering/ssbo/mesh_instance.hpp"

#include <array>
#include <unordered_set>
#include <vector>

namespace polos::scene
{

struct SceneRenderData
{
    std::vector<rendering::MeshInstance> meshInstances;
    std::vector<ecs::EntityHandle>       meshInstanceChanges;
    std::unordered_set<ecs::EntityIndex> pendingMeshUploads;

    rendering::SceneLightInfo                                  lightInfo;
    std::array<rendering::LightInstance, ecs::Settings::kMaxLights> lightInstances;
};

}// namespace polos::scene

#endif// POLOS_SCENE_SCENE_DATA_HPP
