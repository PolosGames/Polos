///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_WINDOWS_HOT_RELOAD_UTILS_HPP
#define POLOS_UTILS_WINDOWS_HOT_RELOAD_UTILS_HPP

#if defined(POLOS_WIN)

namespace polos::utils
{
using LibHandle = HMODULE;// this is also void* under the hood, but whatever, i am a syntax b*tch
}

#    include "polos/logging/log_macros.hpp"

#    include <filesystem>
#    include <string>

#    define NOMINMAX
#    include <windows.h>

namespace polos::utils
{

struct BaseSharedLibOut
{
    utils::LibHandle handle;

    std::filesystem::file_time_type lastWriteTime;
    std::string                     tempDllPath;
};

inline void CleanupOldFiles(const std::filesystem::path& tDir, const std::string& tBaseName)
{
    LogTrace("Cleaning up old DLL and PDB files...");
    for (const auto& entry : std::filesystem::directory_iterator(tDir))
    {
        std::string filename = entry.path().filename().string();
        // Check for temporary DLLs: polos_rendering_*.dll
        if (filename.rfind(tBaseName + "_", 0) == 0 && entry.path().extension() == ".dll")
        {
            std::error_code ec;
            std::filesystem::remove(entry.path(), ec);
            if (ec)
            {
                LogError("Could not remove {}: {}", entry.path().string(), ec.message());
            }
        }
        // Check for locked PDBs: polos_rendering.pdb.locked
        if (filename.rfind(tBaseName, 0) == 0 && filename.ends_with(".pdb.locked"))
        {
            std::error_code ec;
            std::filesystem::remove(entry.path(), ec);
            if (ec)
            {
                LogError("Could not remove {}: {}", entry.path().string(), ec.message());
            }
        }
    }
}


inline void UnloadSharedLib(BaseSharedLibOut& tDllOut)
{
    if (tDllOut.handle)
    {
        FreeLibrary(tDllOut.handle);
        tDllOut.handle = nullptr;
    }
    // Clean up the temporary DLL copy
    if (!tDllOut.tempDllPath.empty())
    {
        std::error_code ec;
        std::filesystem::remove(tDllOut.tempDllPath, ec);
        tDllOut.tempDllPath.clear();
    }
}

// Copy and Load
inline bool LoadSharedLib(BaseSharedLibOut& tDllOut, const std::string& tOriginalDllPathStr)
{
    if (nullptr != tDllOut.handle)
    {
        LogError("DLL Already loaded");
        return false;
    }

    std::filesystem::path original_dll_path(tOriginalDllPathStr);
    if (!std::filesystem::exists(original_dll_path))
    {
        LogWarn("Original DLL not found.");
        return false;
    }

    tDllOut.lastWriteTime = std::filesystem::last_write_time(original_dll_path);

    // Check if a temporary dll already exists

    if (tDllOut.tempDllPath.empty())
    {
        // Create a unique name for the temporary DLL
        std::string           timestamp = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        std::string           temp_dll_name = original_dll_path.stem().string() + "_" + timestamp + ".dll";
        std::filesystem::path temp_dll_path = original_dll_path.parent_path() / temp_dll_name;

        // Copy original to temp location
        std::error_code ec;
        std::filesystem::copy_file(
            original_dll_path,
            temp_dll_path,
            std::filesystem::copy_options::overwrite_existing,
            ec);
        if (ec)
        {
            LogError("Error copying DLL to temp. {}", ec.message());
            return false;
        }
        tDllOut.tempDllPath = temp_dll_path.string();
    }

    // Load the copied DLL
    tDllOut.handle = LoadLibraryA(tDllOut.tempDllPath.c_str());
    if (!tDllOut.handle)
    {
        LogError("Failed to load DLL from {}", tDllOut.tempDllPath);
        return false;
    }

    return true;
}

template<typename F>
inline bool GetFuncFromSharedLib(BaseSharedLibOut& tDllOut, F& tFuncPtr, std::string_view tFuncName)
{
    // Get the address of the exported function
    tFuncPtr = reinterpret_cast<F>(GetProcAddress(tDllOut.handle, tFuncName.data()));
    if (nullptr == tFuncPtr)
    {
        LogError("Failed to get function {} from Shared lib.", std::string(tFuncName));
        FreeLibrary(tDllOut.handle);
        return false;
    }
    return true;
}

}// namespace polos::utils

#endif// POLOS_WIN

#endif// POLOS_UTILS_WINDOWS_HOT_RELOAD_UTILS_HPP
