///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_RESOURCE_HPP
#define POLOS_FILESYSTEM_RESOURCE_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace polos::fs
{

struct alignas(128) resource// NOLINT
{
    std::size_t            uncompressed_size;
    std::size_t            size;
    std::string            stem_name;
    std::string            custom_name;
    std::filesystem::path  path;
    std::vector<std::byte> data;
};

}// namespace polos::fs

#endif// POLOS_FILESYSTEM_RESOURCE_HPP
