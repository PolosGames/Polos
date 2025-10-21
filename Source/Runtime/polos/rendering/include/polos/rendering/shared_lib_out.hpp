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

struct GLFWwindow;

namespace polos::rendering
{

class RenderContext;

namespace
{

#if defined(POLOS_WIN)
inline constexpr char const* kRenderingLibName{"polos_rendering.dll"};
#elif defined(POLOS_LINUX)
inline constexpr char const* kRenderingLibName{"libpolos_rendering.so"};
#endif

}// namespace

struct rendering_shared_lib_out : utils::base_shared_lib_out
{
    RenderContext* context;

    RenderContext* (*create_context_func)(GLFWwindow*);
    void (*destroy_context_func)(RenderContext*);
};

inline bool LoadRenderingModule(rendering_shared_lib_out& t_dll_out)
{
    if (!utils::LoadSharedLib(t_dll_out, kRenderingLibName))
    {
        LogCritical("Could not load {}!", kRenderingLibName);
        return false;
    }

    if (utils::GetFuncFromSharedLib(t_dll_out, t_dll_out.create_context_func, "CreateRenderContext"))
    {
        LogCritical("Could not find symbol CreateRenderContext inside {}", kRenderingLibName);
        return false;
    }
    if (utils::GetFuncFromSharedLib(t_dll_out, t_dll_out.destroy_context_func, "DestroyRenderContext"))
    {
        LogCritical("Could not find symbol DestroyRenderContext inside {}", kRenderingLibName);
        return false;
    }

    return true;
}

inline void UnloadRenderingModule(rendering_shared_lib_out& t_dll_out)
{
    if (t_dll_out.handle != nullptr)
    {
        polos::utils::UnloadSharedLib(t_dll_out);
    }
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHARED_LIB_OUT_HPP_
