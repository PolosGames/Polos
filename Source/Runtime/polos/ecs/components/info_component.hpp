///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_INFO_COMPONENT_HPP
#define POLOS_ECS_INFO_COMPONENT_HPP

#include "polos/ecs/component.hpp"
#include "polos/ecs/scene_graph.hpp"
#include "polos/utils/string_id.hpp"

namespace polos::ecs
{
struct InfoComponent
{
    static constexpr ComponentType kType = ComponentType::kInfo;

    utils::string_id debugName{0U};

    SceneGraphHandle graphHandle{0U};
};
}// namespace polos::ecs

#endif// POLOS_ECS_INFO_COMPONENT_HPP
