///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_BASE_VIEW_HPP
#define POLOS_ECS_BASE_VIEW_HPP

#include "polos/ecs/scene.hpp"
#include "polos/ecs/settings.hpp"

#include <bitset>

namespace polos::ecs
{

template<typename Iterator>
class BaseView
{
public:
    using iterator       = Iterator;
    using const_iterator = Iterator;

    virtual ~BaseView() = default;

    [[nodiscard]] auto begin() const -> iterator
    {
        return iterator(this, 0U);
    }

    [[nodiscard]] auto end() const -> iterator
    {
        return iterator(this, scene->GetEntityPoolSize());
    }

    [[nodiscard]] auto cbegin() const -> const_iterator
    {
        return begin();
    }

    [[nodiscard]] auto cend() const -> const_iterator
    {
        return end();
    }
protected:
    explicit BaseView(Scene* tScene, std::bitset<kMaxComponents> tMask)
        : scene{tScene},
          mask{tMask}
    {}
private:
    friend Iterator;

    [[nodiscard]] auto Matches(EntityIndex tIndex) const -> bool
    {
        auto const* entity = scene->GetEntityDataByIndex(tIndex);
        return entity != nullptr && IsEntityValid(entity->handle) && entity->handle.index == tIndex &&
               (entity->componentMask & mask) == mask;
    }

    Scene*                      scene;
    std::bitset<kMaxComponents> mask;
};

}// namespace polos::ecs

#endif// POLOS_ECS_BASE_VIEW_HPP
