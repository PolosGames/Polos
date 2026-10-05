///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_RESOURCES_ALLOCATION_PARAMS_HPP
#define POLOS_RENDERING_SRC_RESOURCES_ALLOCATION_PARAMS_HPP

#include <vk_mem_alloc.h>

#include <cstdint>

namespace polos::rendering
{

enum class MemoryResidence : std::uint8_t
{
    kHost   = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    kDevice = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,

    kMax = 0x7F,
};

enum class HostAccessFlags : std::uint16_t
{
    kNone     = 0U,
    kSeqWrite = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
    kRandom   = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
};

constexpr std::uint16_t operator&(HostAccessFlags tLhs, HostAccessFlags tRhs)
{ return static_cast<std::uint16_t>(tLhs) & static_cast<std::uint16_t>(tRhs); }

constexpr HostAccessFlags operator|(HostAccessFlags tLhs, HostAccessFlags tRhs)
{ return static_cast<HostAccessFlags>(static_cast<std::uint16_t>(tLhs) | static_cast<std::uint16_t>(tRhs)); }

constexpr HostAccessFlags& operator|=(HostAccessFlags& tLhs, HostAccessFlags tRhs)
{
    tLhs = tLhs | tRhs;
    return tLhs;
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_ALLOCATION_PARAMS_HPP
