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

struct alignas(64) BaseSharedLibOut// NOLINT
{
    utils::LibHandle handle{nullptr};

    std::time_t lastWriteTime{0};
    std::string tempDllPath;
};

inline void CloseLibHandle(LibHandle& tHandle)
{
    if (nullptr != tHandle)
    {
        dlclose(tHandle);
        tHandle = nullptr;
    }
}

inline void RemoveTempFile(std::filesystem::path const& tTempPath)
{
    std::error_code errc;
    if (!std::filesystem::exists(tTempPath))
    {
        return;
    }

    std::filesystem::remove(tTempPath, errc);
    if (errc)
    {
        LogWarn("Failed to remove temp SO {}: {}", tTempPath.string(), errc.message());
    }
    else
    {
        LogDebug("-- Removed temp SO {}", tTempPath.string());
    }
}

inline void UnloadSharedLib(BaseSharedLibOut& tDllOut)
{
    CloseLibHandle(tDllOut.handle);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);

    if (!tDllOut.tempDllPath.empty())
    {
        RemoveTempFile(tDllOut.tempDllPath);
        tDllOut.tempDllPath.clear();
    }
}

inline auto ResolveSharedLibPath(std::filesystem::path const& tPathStr) -> std::filesystem::path
{
    std::filesystem::path original_path(tPathStr);

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

inline auto CopyToTempPath(std::filesystem::path const& tOriginalPath) -> std::filesystem::path
{
    auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();

    std::filesystem::path temp_path =
        tOriginalPath.parent_path() /
        (tOriginalPath.stem().string() + "_hot_" + std::to_string(timestamp) + tOriginalPath.extension().string());

    std::error_code errc;
    std::filesystem::copy_file(tOriginalPath, temp_path, std::filesystem::copy_options::overwrite_existing, errc);

    if (errc)
    {
        LogError("Failed to copy SO to temp path {}: {}", temp_path.string(), errc.message());
        return {};
    }

    return temp_path;
}

inline auto LoadSharedLibHandle(std::filesystem::path const& tTempPath) -> LibHandle
{
    LogDebug("-- Loading temp SO {}...", tTempPath.string());

    LibHandle handle = dlopen(std::filesystem::absolute(tTempPath).c_str(), RTLD_NOW | RTLD_LOCAL);
    if (nullptr == handle)
    {
        LogError("Failed to load SO from {} : {}", tTempPath.string(), std::string(dlerror()));// NOLINT
        return nullptr;
    }

    LogDebug("-- Successfully loaded {}", tTempPath.string());
    return handle;
}

inline bool LoadSharedLib(BaseSharedLibOut& tDllOut, const std::string& tOriginalDllPathStr)
{
    if (nullptr != tDllOut.handle)
    {
        LogWarn("SO file already loaded.");
        return false;
    }

    std::filesystem::path original_dll_path = ResolveSharedLibPath(tOriginalDllPathStr);
    if (original_dll_path.empty())
    {
        LogWarn("SO file not found: {}", tOriginalDllPathStr);
        return false;
    }

    LogDebug("-- Found SO {}, copying to temp...", original_dll_path.string());

    std::filesystem::path temp_path = CopyToTempPath(original_dll_path);
    if (temp_path.empty())
    {
        return false;
    }

    tDllOut.tempDllPath = temp_path.string();
    tDllOut.handle      = LoadSharedLibHandle(temp_path);

    return tDllOut.handle != nullptr;
}

template<typename F>
inline bool GetFuncFromSharedLib(BaseSharedLibOut& tDllOut, F& tFuncPtr, char const* tFuncName)
{
    tFuncPtr = reinterpret_cast<F>(dlsym(tDllOut.handle, tFuncName));// NOLINT
    if (nullptr == tFuncPtr)
    {
        dlclose(tDllOut.handle);

        LogError("Failed to get function {} from Shared lib. {}", tFuncName, dlerror());// NOLINT
        return false;
    }

    LogDebug("-- Successfully got function {}", tFuncName);

    return true;
}

}// namespace polos::utils

#endif// POLOS_LINUX

#endif// POLOS_UTILS_LINUX_HOT_RELOAD_UTILS_HPP
