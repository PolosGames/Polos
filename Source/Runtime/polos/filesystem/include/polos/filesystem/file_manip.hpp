///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_FILE_MANIP_HPP
#define POLOS_FILESYSTEM_FILE_MANIP_HPP

#include "polos/communication/error_code.hpp"
#include "polos/polos_api.hpp"
#include "polos/filesystem/resource.hpp"

#include <filesystem>

namespace polos::fs
{

POLOS_API auto ReadFile(std::filesystem::path const& tFilePath) -> Result<Resource>;
POLOS_API auto ReadFile(std::string_view tCustomName, std::filesystem::path const& tFilePath) -> Result<Resource>;

auto ReadFile(std::string tFilePathe) -> Result<Resource>     = delete;
auto ReadFile(char const* tFilePathe) -> Result<Resource>     = delete;
auto ReadFile(std::string_view tFilePath) -> Result<Resource> = delete;

}// namespace polos::fs

constexpr auto operator""_path(char const* tPathStr, std::size_t /*tSize*/) -> std::filesystem::path
{ return {tPathStr}; }

#endif// POLOS_FILESYSTEM_FILE_MANIP_HPP
