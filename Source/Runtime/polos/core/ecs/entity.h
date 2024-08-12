#pragma once

#define INVALID_ENTITY ::polos::ecs::CreateEntity(static_cast<::polos::ecs::EntityIndex>(-1), 0)

namespace polos::ecs
{
    using Entity = std::uint64_t;
    using CompMask = std::bitset<k_EntityMaxComponentAmount>;

    using EntityIndex = std::uint32_t;
    using EntityVersion = std::uint32_t;

    struct EntityData
    {
        Entity id{static_cast<Entity>(-1)};
        CompMask mask{0};
    };

    constexpr inline Entity CreateEntity(EntityIndex index, EntityVersion version)
    {
        // Shift the index up 32, and put the version in the bottom
        return (static_cast<Entity>(index) << 32) | static_cast<Entity>(version);
    }
    constexpr inline EntityIndex GetEntityIndex(Entity id)
    {
        // Shift down 32, so we lose the version and get our index
        return id >> 32;
    }
    constexpr inline EntityVersion GetEntityVersion(Entity id)
    {
        // Cast to a 32 bit int to get our version number (losing the top 32 bits)
        return static_cast<EntityVersion>(id);
    }
    constexpr inline bool IsEntityValid(Entity id)
    {
        // Check if the index is our invalid index
        return (id >> 32) != static_cast<EntityIndex>(-1);
    }
} // namespace polos::ecs
