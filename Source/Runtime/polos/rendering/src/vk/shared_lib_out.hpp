///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_SHARED_LIB_OUT_HPP
#define POLOS_RENDERING_SRC_VK_SHARED_LIB_OUT_HPP

#include "polos/polos_config.hpp"

#if defined(POLOS_WIN)
#    include "polos/utils/windows_hot_reload_utils.hpp"
#elif defined(POLOS_LINUX)
#    include "polos/utils/linux_hot_reload_utils.hpp"
#endif

#include "polos/polos_api.hpp"
#include "vk/i_render_context.hpp"

namespace polos::rendering
{

#if defined(POLOS_WIN)
constexpr char const* kRenderingLibName{"polos_rendering_impl.dll"};
#elif defined(POLOS_LINUX)
constexpr char const* kRenderingLibName{"libpolos_rendering_impl.so"};
#endif

struct RenderingSharedLibOut : utils::BaseSharedLibOut// NOLINT
{
    using CreateRenderContextFuncT = IRenderContext* (*)();
    CreateRenderContextFuncT createRenderContext{nullptr};
};

inline bool LoadRenderingModule(RenderingSharedLibOut& tDllOut)
{
    auto lib_path = std::filesystem::current_path() / kRenderingLibName;

    if (std::filesystem::exists(lib_path))
    {
        auto ftime = std::filesystem::last_write_time(lib_path);
        // Convert to a rough count for logging
        auto count = std::chrono::duration_cast<std::chrono::seconds>(ftime.time_since_epoch()).count();
        LogInfo("Requesting load of rendering module: {} (Timestamp: {})", lib_path.string(), count);
    }
    else
    {
        LogCritical("Rendering module not found at: {}", lib_path.string());
        return false;
    }

    if (!utils::LoadSharedLib(tDllOut, lib_path.string()))
    {
        LogCritical("Could not load {}!", lib_path.string());
        return false;
    }

    if (!utils::GetFuncFromSharedLib(tDllOut, tDllOut.createRenderContext, "CreateRenderContext"))
    {
        LogCritical("Could not find symbol CreateRenderContext inside {}", kRenderingLibName);
        return false;
    }

    LogDebug("-- Successfully loaded Rendering module!");

    return true;
}

inline void UnloadRenderingModule(RenderingSharedLibOut& tDllOut)
{
    polos::utils::UnloadSharedLib(tDllOut);

    LogDebug("-- Successfully unloaded Rendering module!");
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_SHARED_LIB_OUT_HPP
