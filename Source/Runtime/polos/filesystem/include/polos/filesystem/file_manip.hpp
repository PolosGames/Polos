///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_
#define POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_

#include "polos/filesystem/module_macros.hpp"
#include "polos/filesystem/resource.hpp"

#include <expected>
#include <fstream>

namespace polos::fs
{
FILESYSTEM_EXPORT auto ReadFile(const char*        t_file_path,
                                std::ios::openmode t_open_mode) -> std::expected<resource, bool>;
}

#endif// POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_
