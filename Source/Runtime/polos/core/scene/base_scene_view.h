#pragma once

#include "polos/core/scene/scene.h"
#include "polos/core/ecs/entity.h"

namespace polos
{
    template<typename Iterator>
    class BaseSceneView
    {
        using iterator = Iterator;
        using const_iterator = Iterator const;
    public:
        virtual ~BaseSceneView() = default;

        PL_NODISCARD auto begin() noexcept -> iterator;
        PL_NODISCARD auto end() noexcept -> iterator;

        PL_NODISCARD auto begin() const noexcept -> const_iterator;
        PL_NODISCARD auto end() const noexcept -> const_iterator;

        PL_NODISCARD auto cbegin() const noexcept -> const_iterator;
        PL_NODISCARD auto cend() const noexcept -> const_iterator;

        Scene*        scene{};
        ecs::CompMask mask;
        bool          iterateAll{};
    };
} // namespace polos

#include "polos/core/scene/base_scene_view.inl.h"
