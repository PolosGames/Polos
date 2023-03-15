#pragma once

#include "polos/core/scene/scene.h"
#include "polos/core/ecs/entity.h"
#include "polos/core/scene/base_scene_view.h"
#include "polos/core/scene/scene_view_iterator.h"
#include "polos/core/ecs/ecs_component.h"

namespace polos
{
    template<typename... T> class SceneView;

    template<ecs::EcsComponent... Components>
    class SceneView<Components...> : public base_scene_view
    {
        using iterator = SceneViewIterator<Components...>;
        using const_iterator = iterator const;
    public:
        SceneView(Scene& p_Scene);

        PL_NODISCARD auto begin() noexcept -> iterator;
        PL_NODISCARD auto end() noexcept -> iterator;

        PL_NODISCARD auto begin() const noexcept -> const_iterator;
        PL_NODISCARD auto end() const noexcept -> const_iterator;

        PL_NODISCARD auto cbegin() const noexcept -> const_iterator;
        PL_NODISCARD auto cend() const noexcept -> const_iterator;
    };

    template<typename T>
    concept IsCommonSet = std::is_base_of_v<ecs::CommonSet, T>;

    template<IsCommonSet T>
    class SceneView<T> : public base_scene_view
    {
    private:
        using iterator = T::IteratorType;
        using const_iterator = iterator const;
    private:
        static constexpr std::size_t k_ComponentCount = std::tuple_size_v<iterator::Types>;
    public:
        SceneView(Scene& p_Scene);

        PL_NODISCARD auto begin() noexcept -> iterator;
        PL_NODISCARD auto end() noexcept -> iterator;

        PL_NODISCARD auto begin() const noexcept -> const_iterator;
        PL_NODISCARD auto end() const noexcept -> const_iterator;

        PL_NODISCARD auto cbegin() const noexcept -> const_iterator;
        PL_NODISCARD auto cend() const noexcept -> const_iterator;
    };
} // namespace polos

#include "scene_view.inl.h"
