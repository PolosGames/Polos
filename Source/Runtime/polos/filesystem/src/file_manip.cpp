///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/filesystem/file_manip.hpp"

#include "polos/filesystem/filesystem_error_domain.hpp"
#include "polos/logging/log_macros.hpp"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace polos::fs
{

auto ReadFile(std::filesystem::path const& t_file_path) -> Result<resource>
{
    return ReadFile(t_file_path.filename().string(), t_file_path);
}

auto ReadFile(std::string_view const t_custom_name, std::filesystem::path const& t_file_path) -> Result<resource>
{
    std::string const file_name = t_file_path.filename().string();
    LogDebug("Reading file: {}", file_name);

    std::ifstream file{t_file_path, std::ios::ate | std::ios::binary};

    if (!file.is_open())
    {
        LogError("Could not open file: {}", file_name);
        return ErrorType{FilesystemErrc::kFileWasNotFound};
    }

    if (file.fail())
    {
        LogError("{}, File: {}", std::strerror(errno), file_name);// NOLINT
        return ErrorType{FilesystemErrc::kFileOpenError};
    }
    LogDebug("File has been opened successfully.");

    std::size_t const size = static_cast<std::size_t>(file.tellg());
    LogDebug("File size: {}", size);

    std::vector<std::byte> data{size};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));// NOLINT
    LogDebug("Expected file read was {} bytes, got {} bytes", size, file.gcount());

    return resource{
        .uncompressed_size = size,
        .size              = size,
        .stem_name         = t_file_path.stem().string(),
        .custom_name       = std::string(t_custom_name),
        .path              = t_file_path,
        .data              = data,
    };
}

}// namespace polos::fs
