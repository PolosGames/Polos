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
    static constexpr std::size_t kMaxEntities = 1'048'575U;
    static constexpr std::size_t kMaxCameras  = 16U;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SETTINGS_HPP
