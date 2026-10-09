///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_FILE_MANIP_HPP
#define POLOS_FILESYSTEM_FILE_MANIP_HPP

#include "polos/filesystem/file.hpp"
#include "polos/polos_api.hpp"
#include "polos/utils/error_code.hpp"

#include <filesystem>

namespace polos::fs
{

POLOS_API auto ReadFile(std::filesystem::path const& tFilePath) -> Result<File>;
POLOS_API auto ReadFile(std::string_view tCustomName, std::filesystem::path const& tFilePath) -> Result<File>;

auto ReadFile(std::string tFilePathe) -> Result<File>     = delete;
auto ReadFile(char const* tFilePathe) -> Result<File>     = delete;
auto ReadFile(std::string_view tFilePath) -> Result<File> = delete;

}// namespace polos::fs

constexpr auto operator""_path(char const* tPathStr, std::size_t /*tSize*/) -> std::filesystem::path
{ return {tPathStr}; }

#endif// POLOS_FILESYSTEM_FILE_MANIP_HPP
