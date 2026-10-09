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

template<Component... C>
class ViewIterator
{
public:
    using value_type        = std::conditional_t<sizeof...(C) == 0, EntityHandle, std::tuple<C*...>>;
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

    [[nodiscard]] auto operator*() const -> value_type
    {
        auto const handle = mView->scene->GetEntityAtIndex(mIndex);
        if constexpr (sizeof...(C) == 0)
        {
            return handle;
        }
        else
        {
            return {mView->scene->template GetComponent<C>(handle)...};
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
