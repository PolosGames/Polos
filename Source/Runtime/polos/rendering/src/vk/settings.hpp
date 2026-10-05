///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_SETTINGS_HPP
#define POLOS_RENDERING_SRC_VK_SETTINGS_HPP

#include <cstdlib>

struct Settings
{
    static constexpr bool        kEnableValidationLayers{true};
    static constexpr std::size_t kMaxFramesInFlight{3U};
};

#endif// POLOS_RENDERING_SRC_VK_SETTINGS_HPP
