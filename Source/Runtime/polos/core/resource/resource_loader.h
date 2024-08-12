#pragma once

#include "polos/containers/delegate.h"
#include "polos/core/resource/import_options.h"

namespace polos::resource
{
    template<typename T>
    class ResourceLoader
    {
    public:
        using ResourceType = T;
    public:
        ResourceLoader(Delegate<ResourceType*(std::string, import_options<ResourceType>, ResourceType*)> p_LoadFn, Delegate<bool(std::string)> p_CanImportFn);

        auto ImportResource(std::string p_Path, import_options<ResourceType> p_Options, ResourceType* p_Ptr) -> ResourceType*;
        auto CanImport(std::string p_Path) -> bool;
    private:
        Delegate<ResourceType*(std::string, import_options<ResourceType>, ResourceType*)> m_ImportFn;
        Delegate<bool(std::string)>                                                       m_CanImportFn;
    };
} // namespace polos::resource

#include "resource_loader.inl"
