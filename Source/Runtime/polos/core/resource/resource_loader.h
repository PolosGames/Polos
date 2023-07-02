#pragma once

#include "polos/containers/delegate.h"

namespace polos::resource
{
    template<typename T>
    class ResourceLoader;

    template<typename T>
    class ResourceLoader
    {
    public:
        using ResourceType = T;
    public:
        ResourceLoader(Delegate<bool(std::string)> p_CanLoadFn, Delegate<ResourceType*(std::string, ResourceType*)> p_LoadFn);

        auto LoadResource(std::string p_Path, ResourceType* p_Ptr) -> ResourceType*;
        auto CanLoad(std::string p_Path) -> bool;
    private:
        Delegate<ResourceType*(std::string, ResourceType*)> m_LoadFn;
        Delegate<bool(std::string)>                  m_CanLoadFn;
    };
} // namespace polos::resource

#include "resource_loader.inl"
