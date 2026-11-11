///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SYSTEMS_DIRTY_TRANSFORM_SYSTEM_HPP
#define POLOS_ECS_SYSTEMS_DIRTY_TRANSFORM_SYSTEM_HPP

#include "polos/ecs/entity.hpp"
#include "polos/polos_api.hpp"

namespace polos::ecs
{
class Scene;

class POLOS_API DirtyTransformSystem
{
public:
    void Update(Scene* tScene);
private:
    static void updateInstance(Scene& tScene, EntityHandle tEntity);
};
}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEMS_DIRTY_TRANSFORM_SYSTEM_HPP
