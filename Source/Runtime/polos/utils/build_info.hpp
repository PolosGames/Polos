///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_BUILD_INFO_HPP
#define POLOS_UTILS_BUILD_INFO_HPP

#include "polos/polos_api.hpp"

#include <cstdint>
#include <format>
#include <string>

namespace polos::utils
{

struct BuildInfo
{
    std::uint32_t commitCount{0U};// git rev-list --count HEAD, same on every clone
    std::uint32_t buildNumber{0U};// local to one build directory, +1 per rebuild
    char const*   commit{""};     // short sha, "-dirty" when the tree has changes
    char const*   timestamp{""};  // UTC, "%Y-%m-%d %H:%M"
    char const*   version{""};    // semantic version from the root VERSION file
};

/// Build and version information for the entire engine.
POLOS_API auto GetBuildInfo() -> BuildInfo const&;

inline auto FormatBuildInfo(BuildInfo const& tInfo) -> std::string
{
    return std::format(
        "{} (build {}.{}) | {} | {}",
        tInfo.version,
        tInfo.commitCount,
        tInfo.buildNumber,
        tInfo.timestamp,
        tInfo.commit);
}

}// namespace polos::utils

#endif// POLOS_UTILS_BUILD_INFO_HPP
