///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_MESH_RENDERER_COMPONENT_HPP
#define POLOS_ECS_MESH_RENDERER_COMPONENT_HPP

#include "polos/ecs/component.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"

namespace polos::ecs
{
struct MeshRendererComponent
{
    static constexpr ComponentType kType = ComponentType::kMeshRenderer;

    rendering::MeshHandle meshHandle;
};
}// namespace polos::ecs

#endif// POLOS_ECS_MESH_RENDERER_COMPONENT_HPP
