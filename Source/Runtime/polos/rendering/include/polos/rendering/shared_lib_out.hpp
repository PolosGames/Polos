//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHARED_LIB_OUT_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHARED_LIB_OUT_HPP_

#include "polos/polos_config.hpp"

#if defined(POLOS_WIN)
#    include "polos/utils/windows_hot_reload_utils.hpp"
#elif defined(POLOS_LINUX)
#    include "polos/utils/linux_hot_reload_utils.hpp"
#endif

#include "polos/rendering/i_render_context.hpp"
#include "polos/rendering/module_macros.hpp"

struct GLFWwindow;

namespace polos::rendering
{

#if defined(POLOS_WIN)
inline constexpr char const* kRenderingLibName{"polos_rendering.dll"};
#elif defined(POLOS_LINUX)
inline constexpr char const* kRenderingLibName{"libpolos_rendering_impl.so"};
#endif

struct rendering_shared_lib_out : utils::base_shared_lib_out
{
    using CreateRenderContextFuncT = IRenderContext* (*)();
    CreateRenderContextFuncT CreateRenderContext{nullptr};
};

inline bool LoadRenderingModule(rendering_shared_lib_out& t_dll_out)
{
    if (!utils::LoadSharedLib(t_dll_out, kRenderingLibName))
    {
        LogCritical("Could not load {}!", kRenderingLibName);
        return false;
    }

    if (!utils::GetFuncFromSharedLib(t_dll_out, t_dll_out.CreateRenderContext, "CreateRenderContext"))
    {
        LogCritical("Could not find symbol CreateRenderContext inside {}", kRenderingLibName);
        return false;
    }

    LogDebug("-- Successfully loaded Rendering module!");

    return true;
}

inline void UnloadRenderingModule(rendering_shared_lib_out& t_dll_out)
{
    polos::utils::UnloadSharedLib(t_dll_out);

    LogDebug("-- Successfully unloaded Rendering module!");
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHARED_LIB_OUT_HPP_
