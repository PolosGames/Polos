///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SYSTEMS_LIGHT_COLLECTION_SYSTEM_HPP
#define POLOS_ECS_SYSTEMS_LIGHT_COLLECTION_SYSTEM_HPP

#include "polos/polos_api.hpp"

namespace polos::ecs
{
class Scene;

class POLOS_API LightCollectionSystem
{
public:
    void Update(Scene* tScene);
};
}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEMS_LIGHT_COLLECTION_SYSTEM_HPP
