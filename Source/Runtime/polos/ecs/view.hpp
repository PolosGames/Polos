///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_VIEW_HPP
#define POLOS_ECS_VIEW_HPP

#include "polos/ecs/base_view.hpp"
#include "polos/ecs/component.hpp"
#include "polos/ecs/view_iterator.hpp"

#include <bitset>
#include <cstddef>

namespace polos::ecs
{

/// Iterates tuples of component pointers for live entities containing every requested component.
/// View<> instead iterates the handles of all live entities.
template<Component... C>
class View : public BaseView<ViewIterator<C...>>
{
public:
    View() = delete;

    explicit View(Scene* tScene)
        : BaseView<ViewIterator<C...>>{tScene, MakeMask()}
    {}

    ~View() override = default;
private:
    static auto MakeMask() -> std::bitset<Settings::kMaxComponents>
    {
        std::bitset<Settings::kMaxComponents> mask;
        (mask.set(static_cast<std::size_t>(C::kType)), ...);
        return mask;
    }
};

}// namespace polos::ecs

#endif// POLOS_ECS_VIEW_HPP
