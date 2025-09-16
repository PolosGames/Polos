///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_
#define POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_

#include "polos/filesystem/module_macros.hpp"
#include "polos/filesystem/resource.hpp"

#include <expected>
#include <filesystem>

namespace polos::fs
{

FILESYSTEM_EXPORT auto ReadFile(std::filesystem::path t_file_path) -> std::expected<resource, bool>;

inline auto ReadFile(std::string t_file_pathe) -> std::expected<resource, bool>     = delete;
inline auto ReadFile(char const* t_file_pathe) -> std::expected<resource, bool>     = delete;
inline auto ReadFile(std::string_view t_file_path) -> std::expected<resource, bool> = delete;

}// namespace polos::fs

inline const std::filesystem::path operator""_path(char const* t_path_str, std::size_t)
{
    return std::filesystem::path(t_path_str);
}

#endif// POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILE_MANIP_HPP_
