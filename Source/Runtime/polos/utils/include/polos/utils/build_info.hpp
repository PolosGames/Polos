///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_BUILD_INFO_HPP
#define POLOS_UTILS_BUILD_INFO_HPP

#include <cstdint>
#include <format>
#include <string>

namespace polos::utils
{

struct BuildInfo
{
    std::uint32_t commitCount{0U}; // git rev-list --count HEAD, same on every clone
    std::uint32_t buildNumber{0U}; // local to one build directory, +1 per rebuild
    char const*   commit{""};      // short sha, "-dirty" when the tree has changes
    char const*   timestamp{""};   // UTC, "%Y-%m-%d %H:%M"
    char const*   module{""};      // which binary this copy was linked into
};

/// Resolves to the enclosing shared object's own build, not the process's.
auto GetBuildInfo() -> BuildInfo const&;

inline auto FormatBuildInfo(BuildInfo const& tInfo) -> std::string
{ return std::format("{}.{} | {} | {}", tInfo.commitCount, tInfo.buildNumber, tInfo.timestamp, tInfo.commit); }

}// namespace polos::utils

#endif// POLOS_UTILS_BUILD_INFO_HPP
