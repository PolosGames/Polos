///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SETTINGS_HPP
#define POLOS_ECS_SETTINGS_HPP

#include "polos/polos_config.hpp"

#include <cstddef>

namespace polos::ecs
{
struct Settings
{
    static constexpr size_t kMaxEntities = POLOS_ENTITY_CAPACITY;
    static constexpr size_t kMaxCameras  = 16U;
    static constexpr size_t kMaxLights   = 64U;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SETTINGS_HPP
