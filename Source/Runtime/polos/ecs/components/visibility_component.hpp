///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_VISIBILITY_COMPONENT_HPP
#define POLOS_ECS_VISIBILITY_COMPONENT_HPP

#include "polos/ecs/component.hpp"

#include <format>
#include <string_view>

namespace polos::ecs
{

struct VisibilityComponent
{
    static constexpr ComponentType kType = ComponentType::kVisibility;

    bool isVisible{true};
};

}// namespace polos::ecs

template<>
struct std::formatter<polos::ecs::VisibilityComponent> : std::formatter<std::string_view>
{
    auto format(polos::ecs::VisibilityComponent const& tComponent, std::format_context& tCtx) const
    {
        return std::formatter<std::string_view>::format(tComponent.isVisible ? "Is Visible true" : "Is Visible false", tCtx);
    }
};

#endif// POLOS_ECS_VISIBILITY_COMPONENT_HPP
