namespace polos
{
    template<typename Iterator>
    inline auto BaseSceneView<Iterator>::begin() noexcept -> iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        uint32_t i{};
        while (
            i < scene->GetEntityPoolSize() &&
            (mask != (mask & scene->GetEntityDataByIndex(i).mask) || !ecs::IsEntityValid(scene->GetEntityByIndex(i))))
        {
            i++;
        }

        return iterator(reinterpret_cast<BaseSceneView*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<typename Iterator>
    inline auto BaseSceneView<Iterator>::end() noexcept -> iterator
    {
        return iterator(reinterpret_cast<BaseSceneView*>(this), static_cast<ecs::EntityIndex>(k_SceneMaxEntityAmount));
    }

    template<typename Iterator>
    inline auto BaseSceneView<Iterator>::begin() const noexcept -> const_iterator
    {
        // We have to skip the entities that doesnt match our criteria.
        std::size_t i{};
        while (
            i < scene->GetEntityPoolSize() &&
            (mask != (mask & scene->GetEntityDataByIndex(i).mask) || !ecs::IsEntityValid(scene->GetEntityByIndex(i))))
        {
            i++;
        }
        return const_iterator(reinterpret_cast<BaseSceneView const*>(this), static_cast<ecs::EntityIndex>(i));
    }

    template<typename Iterator>
    inline auto BaseSceneView<Iterator>::end() const noexcept -> const_iterator
    {
        return const_iterator(reinterpret_cast<BaseSceneView const*>(this), static_cast<ecs::EntityIndex>(k_SceneMaxEntityAmount));
    }

    template<typename Iterator>
    inline auto BaseSceneView<Iterator>::cbegin() const noexcept -> const_iterator
    {
        return begin();
    }

    template<typename Iterator>
    inline auto BaseSceneView<Iterator>::cend() const noexcept -> const_iterator
    {
        return end();
    }
} // namespace polos
