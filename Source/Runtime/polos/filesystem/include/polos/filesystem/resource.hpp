//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_RESOURCE_HPP_
#define POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_RESOURCE_HPP_

#include <filesystem>
#include <string>
#include <vector>

namespace polos::fs
{

struct resource
{
    std::size_t            uncompressed_size;
    std::size_t            size;
    std::string            stem_name;
    std::string            custom_name;
    std::filesystem::path  path;
    std::vector<std::byte> data;
};

}// namespace polos::fs

#endif// POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_RESOURCE_HPP_
