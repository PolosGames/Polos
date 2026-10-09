///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SETTINGS_HPP
#define POLOS_RENDERING_SETTINGS_HPP

#include <cstdint>
#include <cstdlib>

struct Settings
{
    static constexpr bool          kEnableValidationLayers{true};
    static constexpr std::size_t   kMaxFramesInFlight{3U};
    static constexpr std::uint32_t kMaxQuads{20000U};
    static constexpr std::uint32_t kMaxSamplers{32U};
    static constexpr std::uint32_t kMaxBatches{8U};
};

#endif// POLOS_RENDERING_SETTINGS_HPP
