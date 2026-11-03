///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_ENTITY_HPP
#define POLOS_ECS_ENTITY_HPP

#include "polos/ecs/component.hpp"
#include "polos/ecs/settings.hpp"
#include "polos/polos_api.hpp"

#include <array>
#include <bitset>
#include <cstdint>
#include <limits>
#include <vector>

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

auto POLOS_API IsEntityValid(EntityHandle tHandle) -> bool;

struct EntityData
{
    EntityHandle                handle;
    std::bitset<kMaxComponents> componentMask;
};

using Entity = EntityData;

struct SerializedEntity
{
    std::uint8_t                              componentCount{0U};
    std::uint32_t                             componentTypes{0U};
    std::array<std::uint32_t, kMaxComponents> componentIndices;
};

}// namespace polos::ecs

auto operator<<(std::vector<polos::ecs::SerializedEntity>& tEntities, polos::ecs::Entity const& tEntity)
    -> std::vector<polos::ecs::SerializedEntity>&;

inline constexpr auto operator==(polos::ecs::EntityHandle const& tLhs, polos::ecs::EntityHandle const& tRhs) -> bool
{
    return tLhs.index == tRhs.index && tLhs.version == tRhs.version;
}

inline constexpr auto operator!=(polos::ecs::EntityHandle const& tLhs, polos::ecs::EntityHandle const& tRhs) -> bool
{
    return !operator==(tLhs, tRhs);
}

#endif// POLOS_ECS_ENTITY_HPP
