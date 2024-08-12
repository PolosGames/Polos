#pragma once

namespace polos::resource
{
    template<typename T>
    ResourceLoader<T>::ResourceLoader(Delegate<ResourceType* (std::string, import_options<ResourceType>, ResourceType*)> p_ImportFn, Delegate<bool(std::string)> p_CanImportFn)
        : m_ImportFn(std::move(p_ImportFn)), m_CanImportFn(std::move(p_CanImportFn))
    {}

    template<typename T>
    auto ResourceLoader<T>::ImportResource(std::string p_Path, import_options<ResourceType> p_Options, ResourceType* p_Ptr) -> ResourceLoader<T>::ResourceType*
    {
        PL_ASSERT(p_Ptr != nullptr, "The data that was passed by AssetCache was nullptr.");

        return m_ImportFn(std::forward<std::string>(p_Path), std::forward<import_options<ResourceType>>(p_Options), std::forward<ResourceType*>(p_Ptr));
    }

    template<typename T>
    auto ResourceLoader<T>::CanImport(std::string p_Path) -> bool
    {
        bool result = m_CanImportFn(std::forward<std::string>(p_Path));

        if (!result)
            LOG_ENGINE_WARN("[ResourceLoader::CanImport] Extension is not supported by the loader.");

        return result;
    }
} // namespace polos::resource