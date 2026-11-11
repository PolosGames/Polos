///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_VIEW_ITERATOR_HPP
#define POLOS_ECS_VIEW_ITERATOR_HPP

#include "polos/ecs/base_view.hpp"
#include "polos/ecs/component.hpp"
#include "polos/ecs/entity.hpp"

#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>

namespace polos::ecs
{

template<typename... C>
class ViewIterator
{
    static constexpr bool kHandleOnly = sizeof...(C) == 0 || std::same_as<std::tuple<C...>, std::tuple<EntityHandle>>;

    template<typename T>
    using Element = std::conditional_t<std::same_as<T, EntityHandle>, EntityHandle, T*>;
public:
    using value_type        = std::conditional_t<kHandleOnly, EntityHandle, std::tuple<Element<C>...>>;
    using reference         = value_type;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept  = std::forward_iterator_tag;

    ViewIterator() = default;

    ViewIterator(BaseView<ViewIterator> const* tView, EntityIndex tIndex)
        : mIndex{tIndex},
          mView{tView}
    {
        SkipNonMatching();
    }

    [[nodiscard]] static auto MakeMask() -> std::bitset<kMaxComponents>
    {
        std::bitset<kMaxComponents> mask;
        (
            [&] {
                if constexpr (!std::same_as<C, EntityHandle>)
                {
                    mask.set(static_cast<std::size_t>(C::kType));
                }
            }(),
            ...);
        return mask;
    }

    [[nodiscard]] auto operator*() const -> value_type
    {
        auto const handle = mView->scene->GetEntityAtIndex(mIndex);
        if constexpr (kHandleOnly)
        {
            return handle;
        }
        else
        {
            return {GetElement<C>(handle)...};
        }
    }

    [[nodiscard]] auto operator==(ViewIterator const& tOther) const -> bool = default;

    auto operator++() -> ViewIterator&
    {
        ++mIndex;
        SkipNonMatching();
        return *this;
    }

    auto operator++(int) -> ViewIterator
    {
        auto previous = *this;
        ++(*this);
        return previous;
    }
private:
    template<typename T>
    [[nodiscard]] auto GetElement(EntityHandle tHandle) const -> Element<T>
    {
        if constexpr (std::same_as<T, EntityHandle>)
        {
            return tHandle;
        }
        else if constexpr (std::is_const_v<T>)
        {
            return mView->scene->template GetComponent<std::remove_const_t<T>>(tHandle);
        }
        else
        {
            return mView->scene->template GetMutableComponent<T>(tHandle);
        }
    }

    auto SkipNonMatching() -> void
    {
        while (mIndex < mView->scene->GetEntityPoolSize() && !mView->Matches(mIndex))
        {
            ++mIndex;
        }
    }

    EntityIndex                   mIndex{};
    BaseView<ViewIterator> const* mView{nullptr};
};

}// namespace polos::ecs

#endif// POLOS_ECS_VIEW_ITERATOR_HPP
