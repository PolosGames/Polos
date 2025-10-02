///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_
#define POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/filesystem/module_macros.hpp"
#include "polos/filesystem/resource.hpp"

#include <filesystem>

namespace polos::fs
{

FILESYSTEM_EXPORT auto ReadFile(std::filesystem::path const t_file_path) -> Result<resource>;
FILESYSTEM_EXPORT auto
ReadFile(std::string_view const t_custom_name, std::filesystem::path const t_file_path) -> Result<resource>;

inline auto ReadFile(std::string t_file_pathe) -> Result<resource>     = delete;
inline auto ReadFile(char const* t_file_pathe) -> Result<resource>     = delete;
inline auto ReadFile(std::string_view t_file_path) -> Result<resource> = delete;

}// namespace polos::fs

inline const std::filesystem::path operator""_path(char const* t_path_str, std::size_t)
{
    return std::filesystem::path(t_path_str);
}

#endif// POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_
