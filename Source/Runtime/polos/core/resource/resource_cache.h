#pragma once

#include "polos/core/resource/resource_loader.h"

namespace polos::resource
{
    namespace detail
    {
        struct resource_info
        {
            std::size_t size;
            std::string path;
        };

        auto CreateInvalidResource() -> ResourceHandle;

        auto CreateResource(std::size_t p_ResourceIndex, std::size_t p_LoaderIndex, uint32 p_Hash) -> ResourceHandle;
        auto GetResourceIndex(ResourceHandle p_Handle) -> std::size_t;
        auto GetResourceLoaderIndex(ResourceHandle p_Handle) -> std::size_t;
        auto GetResourceHash(ResourceHandle p_Handle) -> uint32;
    }

    template<typename T>
    class ResourceCache
    {
        using ResourceType = T;
        using LoaderBase = ResourceLoader<ResourceType>;
    public:
        ResourceCache();
        ~ResourceCache();

        void Shutdown();

        template<typename LoaderType, typename... Args>
        static auto NewLoader(Args&&... args) -> void
            requires(std::is_constructible_v<LoaderType, Args...> && std::is_same_v<typename LoaderType::ResourceType, ResourceType>);

        static auto LoadResource(std::string p_Path) -> ResourceHandle;
        static auto GetResource(uint32 p_Hash) -> ResourceHandle;
        static auto GetRawResource(uint32 p_Hash) -> ResourceType*;
        static auto DestroyResource(uint32 p_Hash) -> bool;
        static auto DestroyAllResources() -> void;
    private:
        static ResourceCache<T>* s_Instance;

        Map<ResourceHandle, detail::resource_info> m_Resources;
        DArray<UniquePtr<LoaderBase>>              m_Loaders;
        DArray<T>                                  m_ResourceCache;
    };
} // namespace polos::resource

#include "resource_cache.inl"
