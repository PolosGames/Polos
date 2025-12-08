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

struct base_shared_lib_out
{
    utils::LibHandle handle;

    std::filesystem::file_time_type last_write_time;
    std::string                     temp_dll_path;
};

inline void CleanupOldFiles(const std::filesystem::path& dir, const std::string& base_name)
{
    LogTrace("Cleaning up old DLL and PDB files...");
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        std::string filename = entry.path().filename().string();
        // Check for temporary DLLs: polos_rendering_*.dll
        if (filename.rfind(base_name + "_", 0) == 0 && entry.path().extension() == ".dll")
        {
            std::error_code ec;
            std::filesystem::remove(entry.path(), ec);
            if (ec)
            {
                LogError("Could not remove {}: {}", entry.path().string(), ec.message());
            }
        }
        // Check for locked PDBs: polos_rendering.pdb.locked
        if (filename.rfind(base_name, 0) == 0 && filename.ends_with(".pdb.locked"))
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


inline void UnloadSharedLib(base_shared_lib_out& t_dll_out)
{
    if (t_dll_out.handle)
    {
        FreeLibrary(t_dll_out.handle);
        t_dll_out.handle = nullptr;
    }
    // Clean up the temporary DLL copy
    if (!t_dll_out.temp_dll_path.empty())
    {
        std::error_code ec;
        std::filesystem::remove(t_dll_out.temp_dll_path, ec);
        t_dll_out.temp_dll_path.clear();
    }
}

// Copy and Load
inline bool LoadSharedLib(base_shared_lib_out& t_dll_out, const std::string& t_original_dll_path_str)
{
    if (nullptr != t_dll_out.handle)
    {
        LogError("DLL Already loaded");
        return false;
    }

    std::filesystem::path original_dll_path(t_original_dll_path_str);
    if (!std::filesystem::exists(original_dll_path))
    {
        LogWarn("Original DLL not found.");
        return false;
    }

    t_dll_out.last_write_time = std::filesystem::last_write_time(original_dll_path);

    // Check if a temporary dll already exists

    if (t_dll_out.temp_dll_path.empty())
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
        t_dll_out.temp_dll_path = temp_dll_path.string();
    }

    // Load the copied DLL
    t_dll_out.handle = LoadLibraryA(t_dll_out.temp_dll_path.c_str());
    if (!t_dll_out.handle)
    {
        LogError("Failed to load DLL from {}", t_dll_out.temp_dll_path);
        return false;
    }

    return true;
}

template<typename F>
inline bool GetFuncFromSharedLib(base_shared_lib_out& t_dll_out, F& t_func_ptr, std::string_view t_func_name)
{
    // Get the address of the exported function
    t_func_ptr = reinterpret_cast<F>(GetProcAddress(t_dll_out.handle, t_func_name.data()));
    if (nullptr == t_func_ptr)
    {
        LogError("Failed to get function {} from Shared lib.", std::string(t_func_name));
        FreeLibrary(t_dll_out.handle);
        return false;
    }
    return true;
}

}// namespace polos::utils

#endif// POLOS_WIN

#endif// POLOS_UTILS_WINDOWS_HOT_RELOAD_UTILS_HPP
