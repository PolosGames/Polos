//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DLL_MAIN_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DLL_MAIN_HPP_

#include "polos/rendering/vulkan_state.hpp"
#include "polos/utils/windows_hot_reload_utils.hpp"

struct GLFWwindow;

namespace polos::rendering
{
struct rendering_dll_out : utils::base_dll_out
{
    using initialize_vulkan_func_t = polos::rendering::VulkanState* (*)(GLFWwindow* const);
    using terminate_vulkan_func_t  = void (*)();
    using render_frame_func_t      = void (*)();

    initialize_vulkan_func_t initialize_vulkan_func{nullptr};
    terminate_vulkan_func_t  terminate_vulkan_func{nullptr};
    render_frame_func_t      render_frame_func{nullptr};
};

inline bool LoadRenderingModule(rendering_dll_out& t_dll_out)
{
    if (!polos::utils::LoadDLL(t_dll_out, "polos_rendering.dll"))
    {
        LogError("Could not load rendering dll");
        return false;
    }

    bool ret = utils::GetFuncFromDLL(t_dll_out, t_dll_out.initialize_vulkan_func, "InitializeVulkan");
    ret &= utils::GetFuncFromDLL(t_dll_out, t_dll_out.terminate_vulkan_func, "TerminateVulkan");
    ret &= polos::utils::GetFuncFromDLL(t_dll_out, t_dll_out.render_frame_func, "RenderFrame");

    if (!ret)
    {
        polos::utils::UnloadDLL(t_dll_out);

        LogCritical("Could not find either of the rendering functions!! Leaving.!!");
        return false;
    }

    return true;
}

inline void UnloadRenderingModule(rendering_dll_out& t_dll_out)
{
    if (t_dll_out.handle != nullptr)
    {
        polos::utils::UnloadDLL(t_dll_out);
    }
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DLL_MAIN_HPP_
