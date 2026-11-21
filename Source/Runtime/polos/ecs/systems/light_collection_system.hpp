///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SYSTEMS_LIGHT_COLLECTION_SYSTEM_HPP
#define POLOS_ECS_SYSTEMS_LIGHT_COLLECTION_SYSTEM_HPP

#include "polos/polos_api.hpp"

namespace polos::scene
{
class Scene;
}// namespace polos::scene

namespace polos::ecs
{
class POLOS_API LightCollectionSystem
{
public:
    void Update(scene::Scene* tScene);
};
}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEMS_LIGHT_COLLECTION_SYSTEM_HPP
