///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_VISIBILITY_COMPONENT_HPP
#define POLOS_ECS_VISIBILITY_COMPONENT_HPP

#include "polos/ecs/component.hpp"

#include <quill/DeferredFormatCodec.h>
#include <quill/bundled/fmt/ostream.h>

namespace polos::ecs
{

struct VisibilityComponent
{
    static constexpr ComponentType kType = ComponentType::kVisibility;

    bool isVisible{true};
};

}// namespace polos::ecs

template<>
struct fmtquill::formatter<polos::ecs::VisibilityComponent> : fmtquill::formatter<std::string_view>
{
    auto format(polos::ecs::VisibilityComponent const& tComponent, format_context& tCtx) const
    { return fmtquill::format_to(tCtx.out(), "Is Visible {}", tComponent.isVisible); }
};

template<>
struct quill::Codec<polos::ecs::VisibilityComponent> : quill::DeferredFormatCodec<polos::ecs::VisibilityComponent>
{
};

#endif// POLOS_ECS_VISIBILITY_COMPONENT_HPP
