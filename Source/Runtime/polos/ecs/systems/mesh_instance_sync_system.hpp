///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SYSTEMS_MESH_INSTANCE_SYNC_SYSTEM_HPP
#define POLOS_ECS_SYSTEMS_MESH_INSTANCE_SYNC_SYSTEM_HPP

#include "polos/ecs/entity.hpp"
#include "polos/polos_api.hpp"

namespace polos::scene
{
class Scene;
}// namespace polos::scene

namespace polos::ecs
{
class POLOS_API MeshInstanceSyncSystem
{
public:
    void Update(scene::Scene* tScene);
private:
    static void updateInstance(scene::Scene& tScene, EntityHandle tEntity);
};
}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEMS_MESH_INSTANCE_SYNC_SYSTEM_HPP
