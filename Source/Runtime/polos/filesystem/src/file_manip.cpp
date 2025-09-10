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

auto ReadFile(std::filesystem::path t_file_path) -> std::expected<resource, bool>
{
    std::string file_name = t_file_path.filename().string();
    LogDebug("Reading file: {}", file_name);

    std::ifstream file{t_file_path, std::ios::ate | std::ios::binary};

    if (!file.is_open())
    {
        LogError("Could not open file: {}", file_name);
        return std::unexpected{false};
    }

    if (file.fail())
    {
        LogError("{}, File: {}", std::strerror(errno), file_name);
        return std::unexpected{false};
    }
    LogDebug("File has been opened successfully.");

    std::size_t size = static_cast<std::size_t>(file.tellg());
    LogDebug("File size: {}", size);

    std::vector<std::byte> data{size};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));
    LogDebug("Expected file read was {} bytes, got {} bytes", size, file.gcount());

    return resource{.uncompressed_size = size, .size = size, .name = t_file_path.stem().string(), .data = data};
}

}// namespace polos::fs
