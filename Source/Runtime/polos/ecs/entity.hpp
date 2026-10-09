///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_ENTITY_HPP
#define POLOS_ECS_ENTITY_HPP

#include "polos/ecs/component.hpp"
#include "polos/polos_api.hpp"

#include <bitset>
#include <cstdint>
#include <limits>

namespace polos::ecs
{

using EntityIndex   = std::uint32_t;
using EntityVersion = std::uint32_t;

struct EntityHandle
{
    EntityIndex   index;
    EntityVersion version;
};

inline constexpr EntityIndex  kInvalidEntityIndex{Settings::kMaxEntities};
inline constexpr EntityIndex  kInvalidEntityVersion{std::numeric_limits<std::uint32_t>::max()};
inline constexpr EntityHandle kInvalidEntityHandle{
    .index   = kInvalidEntityIndex,
    .version = kInvalidEntityVersion,
};

struct EntityData
{
    EntityHandle                          handle;
    std::bitset<Settings::kMaxComponents> componentMask;
};

using Entity = EntityData;

auto POLOS_API IsEntityValid(EntityHandle tHandle) -> bool;

}// namespace polos::ecs

#endif// POLOS_ECS_ENTITY_HPP
