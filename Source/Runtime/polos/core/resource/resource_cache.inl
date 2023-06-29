#pragma once

#include "resource_cache.h"

#include "polos/core/engine/engine.h"
#include "polos/core/io/file.h"
#include "polos/core/resource/resource_common.h"

namespace polos::resource
{
    template<typename T>
    ResourceCache<T>* ResourceCache<T>::s_Instance;

    template<typename T>
    ResourceCache<T>::ResourceCache()
    {
        s_Instance = this;

        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);
    }
    
    template<typename T>
    ResourceCache<T>::~ResourceCache()
    {
        s_Instance = nullptr;
    }

    template<typename T>
    void ResourceCache<T>::Shutdown()
    {
        for (auto const& [handle, _] : m_Resources)
            DestroyResource(detail::GetResourceHash(handle));
    }

    template<typename T>
    template<typename LoaderType, typename... Args>
    auto ResourceCache<T>::NewLoader(Args&&... args) -> void
        requires(std::is_constructible_v<LoaderType, Args...> && std::is_same_v<typename LoaderType::ResourceType, ResourceType>)
    {
        s_Instance->m_Loaders.emplace_back(new LoaderType(std::forward<Args>(args)...));
    }

    template<typename T>
    auto ResourceCache<T>::LoadResource(std::string p_Path) -> ResourceHandle
    {
        auto const [extension, name] = [&p_Path] {
            File file(p_Path, k_Read);
            return std::pair(file.fileExtension, file.fileName);
        }();

        LoaderBase* loader{};
        size_t loader_index{};

        for (std::size_t i{}; i != s_Instance->m_Loaders.size(); i++)
        {
            auto& l = s_Instance->m_Loaders[i];
            if (l->CanLoad(p_Path))
            {
                loader = l.get();
                loader_index = i;
            }
        }

        if (loader == nullptr)
        {
            LOG_ENGINE_ERROR("[ResourceCache::LoadResource] No Loader was found that could load resource \"{}\".", typeid(ResourceType).name());
            return INVALID_RESOURCE;
        }

        auto rsc_index = s_Instance->m_ResourceCache.size();
        s_Instance->m_ResourceCache.emplace_back();
        auto* rsc = loader->LoadResource(p_Path, &s_Instance->m_ResourceCache.back());
        
        if (rsc == nullptr)
        {
            LOG_ENGINE_ERROR("[ResourceCache::LoadResource] Couldn't load the resource \"{}\" with the given loader.", typeid(ResourceType).name());
            s_Instance->m_ResourceCache.pop_back();
            return INVALID_RESOURCE;
        }
        
        auto rsc_handle = detail::CreateResource(rsc_index, loader_index, StrHash32(name.c_str()));

        s_Instance->m_Resources.try_emplace(rsc_handle, std::filesystem::file_size(p_Path), p_Path);

        return rsc_handle;
    }

    template<typename T>
    auto ResourceCache<T>::GetResource(uint32 p_Hash) -> ResourceHandle
    {
        for (auto& [handle, _] : s_Instance->m_Resources)
        {
            auto hash = detail::GetResourceHash(handle);
            if (hash == p_Hash)
                return handle;
        }

        return INVALID_RESOURCE;
    }
    
    template<typename T>
    auto ResourceCache<T>::GetRawResource(uint32 p_Hash) -> ResourceType*
    {
        for (auto& [handle, _] : s_Instance->m_Resources)
        {
            auto hash = detail::GetResourceHash(handle);
            if (hash == p_Hash)
            {
                auto index = detail::GetResourceIndex(handle);
                return s_Instance->m_ResourceCache[index];
            }
        }

        return nullptr;
    }
    
    template<typename T>
    auto ResourceCache<T>::DestroyResource(uint32 p_Hash) -> bool
    {
        for (auto& [handle, _] : s_Instance->m_Resources)
        {
            auto hash = detail::GetResourceHash(handle);
            if (hash == p_Hash)
            {
                auto index = detail::GetResourceIndex(handle);
                
                s_Instance->m_ResourceCache.erase(std::next(s_Instance->m_ResourceCache.begin(), index));
                s_Instance->m_Resources.erase(handle);

                return true;
            }
        }

        return false;
    }
} // namespace polos::resource
