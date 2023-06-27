#pragma once

#include "polos/containers/delegate.h"

namespace polos::resource
{
    template<typename T>
    class ResourceLoader;
    
    template<typename T>
    concept ResourceLoadable = requires (T x)
    {
        std::is_base_of_v<ResourceLoader<typename T::ResourceType>, T>;
        { T::Load() } -> std::same_as<std::add_pointer_t<typename T::ResourceType>>;
    };

    template<typename T>
    class ResourceLoader
    {
    public:
        using ResourceType = T;
    public:
        ResourceLoader() = default;

        ResourceLoader(std::vector<std::string> p_SupportedTypes, Delegate<std::add_pointer_t<T>(std::string)> p_LoadFn);

        auto LoadResource(std::string p_Path) -> ResourceType*;
        auto CanLoad(std::string p_ExtensionType) -> bool;
    private:
        std::vector<std::string> m_SupportedTypes;
        Delegate<std::add_pointer_t<T>(std::string)> m_LoadFn;
    };
} // namespace polos::resource

#include "resource_loader.inl"
