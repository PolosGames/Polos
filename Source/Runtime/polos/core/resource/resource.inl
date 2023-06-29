#pragma once

#include "polos/core/resource/resource_common.h"
#include "polos/core/resource/resource_cache.h"

namespace polos::resource
{
    template<typename T>
    auto LoadResource(std::string p_Path) -> ResourceHandle
    {
        return ResourceCache<T>::LoadResource(p_Path);
    }

    template<typename T>
    auto GetResource(std::string p_Name) -> ResourceHandle
    {
        uint32 hash = StrHash32(p_Name.c_str());
        auto handle = ResourceCache<T>::GetResource(hash);

        if (handle == INVALID_RESOURCE)
            LOG_ENGINE_ERROR("[GetResource] Resource with the name \"{}\" was not found", p_Name);

        return handle;
    }

    template<typename T>
    auto GetRawResource(std::string p_Name) -> T*
    {
        uint32 hash = StrHash32(p_Name.c_str());
        auto handle = ResourceCache<T>::GetResource(hash);

        if (handle == INVALID_RESOURCE)
            LOG_ENGINE_ERROR("[GetRawResource] Resource with the name \"{}\" was not found", p_Name);

        return handle;
    }

    template<typename T>
    auto DestroyResource(std::string p_Name) -> void
    {
        uint32 hash = StrHash32(p_Name.c_str());
        auto result = ResourceCache<T>::DestroyResource(hash);

        if(!result)
            LOG_ENGINE_ERROR("[DestroyResource] Resource with the name \"{}\" was not found", p_Name);
    }
} // namespace polos::resource
