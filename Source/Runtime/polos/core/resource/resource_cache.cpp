#include "resource_cache.h"

namespace polos::resource::detail
{
    auto CreateInvalidResource() -> ResourceHandle
    {
        return CreateResource(static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), static_cast<uint32>(-1));
    }

    auto CreateResource(std::size_t p_ResourceIndex, std::size_t p_LoaderIndex, uint32 p_Hash) -> ResourceHandle
    {
        auto indices = static_cast<uint32>(p_ResourceIndex);
        indices = (indices << 16) | static_cast<uint32>(p_LoaderIndex);

        return (static_cast<ResourceHandle>(indices) << 32) | static_cast<ResourceHandle>(p_Hash);
    }

    auto GetResourceIndex(ResourceHandle p_Handle) -> std::size_t
    {
        return static_cast<std::size_t>((p_Handle >> 48));
    }

    auto GetResourceLoaderIndex(ResourceHandle p_Handle) -> std::size_t
    {
        auto indices      = static_cast<uint32>(p_Handle >> 32);
        uint32 loader_index = indices & 0x0000ffff;
        return static_cast<std::size_t>(loader_index);
    }

    auto GetResourceHash(ResourceHandle p_Handle) -> uint32
    {
        return static_cast<uint32>((p_Handle << 32));
    }
} // namespace polos::resource::detail::
