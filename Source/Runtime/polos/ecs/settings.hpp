///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SETTINGS_HPP
#define POLOS_ECS_SETTINGS_HPP

#include <cstddef>
#include <cstdint>

namespace polos::ecs
{
struct Settings
{
    static constexpr std::size_t kMaxEntities = 4'194'304U;// 2^22
    static constexpr std::size_t kMaxCameras  = 16U;
    static constexpr size_t      kMaxLights   = 64U;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SETTINGS_HPP
