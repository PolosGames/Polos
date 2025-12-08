///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP
#define POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP

#include "polos/polos_config.hpp"

#if defined(POLOS_LINUX)

#    include "polos/logging/log_macros.hpp"

#    include <dlfcn.h>

#    include <ctime>
#    include <filesystem>
#    include <string>

namespace polos::utils
{

using LibHandle = void*;

struct alignas(64) base_shared_lib_out// NOLINT
{
    utils::LibHandle handle{nullptr};

    std::time_t last_write_time{0};
    std::string temp_dll_path;
};

inline void UnloadSharedLib(base_shared_lib_out& t_dll_out)
{
    if (nullptr != t_dll_out.handle)
    {
        dlclose(t_dll_out.handle);
        t_dll_out.handle = nullptr;
    }
}

inline bool LoadSharedLib(base_shared_lib_out& t_dll_out, const std::string& t_original_dll_path_str)
{
    if (nullptr != t_dll_out.handle)
    {
        LogWarn("SO file already loaded.");
        return false;
    }

    std::filesystem::path const original_dll_path(t_original_dll_path_str);
    if (!std::filesystem::exists(original_dll_path))
    {
        LogWarn("SO file not found.");
        return false;
    }

    LogDebug("-- Found SO {}, loading...", original_dll_path.string());

    t_dll_out.handle = dlopen(original_dll_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (nullptr == t_dll_out.handle)
    {
        LogError("Failed to load SO from {} : {}", original_dll_path.string(), std::string(dlerror()));// NOLINT
        return false;
    }

    LogDebug("-- Successfully loaded {}", original_dll_path.string());

    return true;
}

template<typename F>
inline bool GetFuncFromSharedLib(base_shared_lib_out& t_dll_out, F& t_func_ptr, char const* t_func_name)
{
    t_func_ptr = reinterpret_cast<F>(dlsym(t_dll_out.handle, t_func_name));// NOLINT
    if (nullptr == t_func_ptr)
    {
        dlclose(t_dll_out.handle);

        LogError("Failed to get function {} from Shared lib. {}", t_func_name, dlerror());// NOLINT
        return false;
    }

    LogDebug("-- Successfully got function {}", t_func_name);

    return true;
}

}// namespace polos::utils

#endif// POLOS_LINUX

#endif// POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP
