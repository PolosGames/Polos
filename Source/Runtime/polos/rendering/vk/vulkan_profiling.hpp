///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_PROFILING_HPP
#define POLOS_RENDERING_VULKAN_PROFILING_HPP

#include "polos/utils/profiling.hpp"

#if defined(ENABLE_PROFILING)
#    include "polos/rendering/vk/render_context.hpp"

#    include <tracy/TracyVulkan.hpp>

#    define POLOS_PROFILE_GPU_ZONE(CommandBuffer, Name)                \
        auto* const TracyConcat(polosGpuContext, __LINE__) =           \
            ::polos::rendering::RenderContext::Get().GetGpuProfileContext(); \
        TracyVkNamedZone(                                              \
            TracyConcat(polosGpuContext, __LINE__),                    \
            TracyConcat(polosGpuZone, __LINE__),                       \
            CommandBuffer,                                             \
            Name,                                                      \
            TracyConcat(polosGpuContext, __LINE__) != nullptr)
#else
#    define POLOS_PROFILE_GPU_ZONE(CommandBuffer, Name) static_cast<void>(0)
#endif

#endif// POLOS_RENDERING_VULKAN_PROFILING_HPP
