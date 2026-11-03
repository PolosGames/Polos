///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/entity.hpp"

#include "polos/ecs/settings.hpp"

namespace polos::ecs
{
auto IsEntityValid(EntityHandle tHandle) -> bool
{
    return tHandle.index < kInvalidEntityIndex && tHandle.version < kInvalidEntityVersion;
}

}// namespace polos::ecs

auto operator<<(std::vector<polos::ecs::SerializedEntity>& tEntities, polos::ecs::EntityData const& tEntity)
    -> std::vector<polos::ecs::SerializedEntity>&
{
    tEntities.push_back(
        polos::ecs::SerializedEntity{
            .componentCount = static_cast<std::uint8_t>(tEntity.componentMask.count()),
            .componentTypes = static_cast<std::uint32_t>(tEntity.componentMask.to_ullong()),
        });

    return tEntities;
}
