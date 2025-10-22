//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP_

#include "polos/polos_config.hpp"

#if defined(POLOS_LINUX)

namespace polos::utils
{
using LibHandle = void*;
}

#    include "polos/logging/log_macros.hpp"

#    include <ctime>
#    include <dlfcn.h>
#    include <filesystem>
#    include <string>

namespace polos::utils
{

struct base_shared_lib_out
{
    utils::LibHandle handle;

    std::time_t last_write_time;
    std::string temp_dll_path;
};

inline void CleanupOldFiles(const std::filesystem::path&, const std::string&)
{
    // NO-OP for linux
}

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

    std::filesystem::path original_dll_path(t_original_dll_path_str);
    if (!std::filesystem::exists(original_dll_path))
    {
        LogWarn("SO file not found.");
        return false;
    }

    t_dll_out.handle = dlopen(original_dll_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (nullptr == t_dll_out.handle)
    {
        LogError("Failed to load SO from {} : {}", original_dll_path.string(), std::string(dlerror()));
        return false;
    }

    return true;
}

template<typename F>
inline bool GetFuncFromSharedLib(base_shared_lib_out& t_dll_out, F& t_func_ptr, char const* t_func_name)
{
    t_func_ptr = reinterpret_cast<F>(dlsym(t_dll_out.handle, t_func_name));
    if (nullptr == t_func_ptr)
    {
        dlclose(t_dll_out.handle);

        LogError("Failed to get function {} from Shared lib. {}", t_func_name, dlerror());
        return false;
    }
    return true;
}

}// namespace polos::utils
#endif// POLOS_LINUX/

#endif// POLOS_UTILS_INCLUDE_POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP_
