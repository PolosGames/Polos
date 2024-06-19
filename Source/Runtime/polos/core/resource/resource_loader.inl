#pragma once

namespace polos::resource
{
    template<typename T>
    ResourceLoader<T>::ResourceLoader(Delegate<bool(std::string)> p_CanLoadFn, Delegate<T*(std::string, T*)> p_LoadFn)
        : m_LoadFn{std::move(p_LoadFn)}, m_CanLoadFn{std::move(p_CanLoadFn)}
    {}

    template<typename T>
    auto ResourceLoader<T>::LoadResource(std::string p_Path, T* p_Ptr) -> ResourceLoader<T>::ResourceType*
    {
        PL_ASSERT(p_Ptr != nullptr, "The data that was passed by AssetCache was nullptr.");

        return m_LoadFn(std::move(p_Path), std::forward<T*>(p_Ptr));
    }

    template<typename T>
    auto ResourceLoader<T>::CanLoad(std::string p_Path) -> bool
    {
        bool result = m_CanLoadFn(std::move(p_Path));

        if (!result)
            LOG_ENGINE_WARN("[ResourceLoader::CanLoad] Extension is not supported by the loader.");

        return result;
    }
} // namespace polos::resource