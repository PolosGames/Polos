//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef FILESYSTEM_FILE_H_
#define FILESYSTEM_FILE_H_

#include <cstdint>
#include <string>
#include <vector>

namespace polos::fs
{

struct resource
{
    std::size_t            uncompressed_size;
    std::size_t            size;
    std::string            name;
    std::vector<std::byte> data;
};

}// namespace polos::fs

#endif//FILESYSTEM_FILE_H_
