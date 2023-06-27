#pragma once

namespace polos::resource
{
    template<typename T>
    ResourceLoader<T>::ResourceLoader(std::vector<std::string> p_SupportedTypes, Delegate<std::add_pointer_t<T>(std::string)> p_LoadFn)
        : m_SupportedTypes{std::move(p_SupportedTypes)}, m_LoadFn{p_LoadFn}
    {}

    template<typename T>
    auto ResourceLoader<T>::LoadResource(std::string p_Path) -> ResourceLoader<T>::ResourceType*
    {
        return m_LoadFn(p_Path);
    }

    template<typename T>
    auto ResourceLoader<T>::CanLoad(std::string p_ExtensionType) -> bool
    {
        for (auto const& type : m_SupportedTypes)
        {
            if (p_ExtensionType == type)
                return true;
        }

        LOG_ENGINE_WARN("Extension \"{}\" is not supported by the loader \"{}\"", p_ExtensionType);
        return false;
    }
} // namespace polos::resource