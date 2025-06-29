///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace polos::fs
{

auto ReadFile(const char* t_file_path, std::ios::openmode t_open_mode) -> std::expected<resource, bool>
{
    LogTrace("[ReadFile]");
    std::filesystem::path file_path{t_file_path};
    std::string           file_name = file_path.filename().string();
    LogDebug("[ReadFile] Reading file: {}", file_name);

    std::ifstream file{file_path, t_open_mode};

    if (!file.is_open())
    {
        LogError("[ReadFile] Could not open file: {}", file_name);
        return std::unexpected{false};
    }

    if (file.fail())
    {
        LogError("[ReadFile] {}, File: {}", std::strerror(errno), file_name);
        return std::unexpected{false};
    }
    LogDebug("[ReadFile] File has been opened successfully.");

    std::size_t size = static_cast<std::size_t>(file.tellg());
    LogDebug("[ReadFile] File size: {}", size);

    std::vector<std::byte> data{size};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));
    LogDebug("[ReadFile] Expected file read was {} bytes, got {} bytes", size, file.gcount());

    return resource{.uncompressed_size = size, .size = size, .name = file_path.stem().string(), .data = data};
}

}// namespace polos::fs
