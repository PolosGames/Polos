///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_VIEW_HPP
#define POLOS_ECS_VIEW_HPP

#include "polos/ecs/base_view.hpp"
#include "polos/ecs/component.hpp"
#include "polos/ecs/view_iterator.hpp"

#include <tuple>
#include <type_traits>

namespace polos::ecs
{

template<Component... C>
struct Read
{
};

template<Component... C>
struct Write
{
};

namespace detail
{
template<typename T>
    requires(Component<T> || std::same_as<T, EntityHandle>)
auto ViewElements(std::type_identity<T>) -> std::tuple<T>;

template<Component... C>
auto ViewElements(std::type_identity<Read<C...>>) -> std::tuple<C const...>;

template<Component... C>
auto ViewElements(std::type_identity<Write<C...>>) -> std::tuple<C...>;

template<typename... C>
auto IteratorFor(std::tuple<C...>) -> ViewIterator<C...>;

template<typename... Access>
using ViewIteratorFor = decltype(IteratorFor(std::tuple_cat(ViewElements(std::type_identity<Access>{})...)));
}// namespace detail

// A handle-only view yields EntityHandle values. Other views yield tuples in
// declaration order, with const pointers for Read and mutable pointers for Write.
// View<EntityHandle>
// View<EntityHandle, Read<TransformComponent>, Write<MeshRendererComponent>>
// View<Write<TransformComponent, MeshRendererComponent>, Read<VisibilityComponent>>
template<typename... Access>
class View : public BaseView<detail::ViewIteratorFor<Access...>>
{
    using Iterator = detail::ViewIteratorFor<Access...>;
public:
    View() = delete;

    explicit View(scene::Scene* tScene)
        : BaseView<Iterator>{tScene, Iterator::MakeMask()}
    {}

    ~View() override = default;
};

}// namespace polos::ecs

#endif// POLOS_ECS_VIEW_HPP
