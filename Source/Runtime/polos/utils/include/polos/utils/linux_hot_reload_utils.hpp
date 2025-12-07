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

#    include <chrono>
#    include <ctime>
#    include <filesystem>
#    include <string>
#    include <thread>

namespace polos::utils
{

using LibHandle = void*;

struct alignas(64) base_shared_lib_out// NOLINT
{
    utils::LibHandle handle{nullptr};

    std::time_t last_write_time{0};
    std::string temp_dll_path;
};

inline void CloseLibHandle(LibHandle& t_handle)
{
    if (nullptr != t_handle)
    {
        dlclose(t_handle);
        t_handle = nullptr;
    }
}

inline void RemoveTempFile(std::filesystem::path const& t_temp_path)
{
    std::error_code errc;
    if (!std::filesystem::exists(t_temp_path))
    {
        return;
    }

    std::filesystem::remove(t_temp_path, errc);
    if (errc)
    {
        LogWarn("Failed to remove temp SO {}: {}", t_temp_path.string(), errc.message());
    }
    else
    {
        LogDebug("-- Removed temp SO {}", t_temp_path.string());
    }
}

inline void UnloadSharedLib(base_shared_lib_out& t_dll_out)
{
    CloseLibHandle(t_dll_out.handle);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);

    if (!t_dll_out.temp_dll_path.empty())
    {
        RemoveTempFile(t_dll_out.temp_dll_path);
        t_dll_out.temp_dll_path.clear();
    }
}

inline auto ResolveSharedLibPath(std::filesystem::path const& t_path_str) -> std::filesystem::path
{
    std::filesystem::path original_path(t_path_str);

    if (std::filesystem::exists(original_path))
    {
        return original_path;
    }

    // Fallback: check in current directory if just a filename was given
    std::filesystem::path fallback_path = std::filesystem::current_path() / original_path;
    if (std::filesystem::exists(fallback_path))
    {
        return fallback_path;
    }

    return {};
}

inline auto CopyToTempPath(std::filesystem::path const& t_original_path) -> std::filesystem::path
{
    auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();

    std::filesystem::path temp_path =
        t_original_path.parent_path() /
        (t_original_path.stem().string() + "_hot_" + std::to_string(timestamp) + t_original_path.extension().string());

    std::error_code errc;
    std::filesystem::copy_file(t_original_path, temp_path, std::filesystem::copy_options::overwrite_existing, errc);

    if (errc)
    {
        LogError("Failed to copy SO to temp path {}: {}", temp_path.string(), errc.message());
        return {};
    }

    return temp_path;
}

inline auto LoadSharedLibHandle(std::filesystem::path const& t_temp_path) -> LibHandle
{
    LogDebug("-- Loading temp SO {}...", t_temp_path.string());

    LibHandle handle = dlopen(std::filesystem::absolute(t_temp_path).c_str(), RTLD_NOW | RTLD_LOCAL);
    if (nullptr == handle)
    {
        LogError("Failed to load SO from {} : {}", t_temp_path.string(), std::string(dlerror()));// NOLINT
        return nullptr;
    }

    LogDebug("-- Successfully loaded {}", t_temp_path.string());
    return handle;
}

inline bool LoadSharedLib(base_shared_lib_out& t_dll_out, const std::string& t_original_dll_path_str)
{
    if (nullptr != t_dll_out.handle)
    {
        LogWarn("SO file already loaded.");
        return false;
    }

    std::filesystem::path original_dll_path = ResolveSharedLibPath(t_original_dll_path_str);
    if (original_dll_path.empty())
    {
        LogWarn("SO file not found: {}", t_original_dll_path_str);
        return false;
    }

    LogDebug("-- Found SO {}, copying to temp...", original_dll_path.string());

    std::filesystem::path temp_path = CopyToTempPath(original_dll_path);
    if (temp_path.empty())
    {
        return false;
    }

    t_dll_out.temp_dll_path = temp_path.string();
    t_dll_out.handle        = LoadSharedLibHandle(temp_path);

    return t_dll_out.handle != nullptr;
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
