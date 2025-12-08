//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/common.hpp"

#include <cstddef>
#include <cstdint>

namespace polos::rendering
{

namespace
{

constexpr std::uint32_t kIndexBits{16U};
constexpr std::uint32_t kTypeBits{4U};
constexpr std::uint32_t kVersionBits{12U};

constexpr std::uint32_t kIndexShift{0U};
constexpr std::uint32_t kTypeShift    = kIndexBits;
constexpr std::uint32_t kVersionShift = kIndexBits + kTypeBits;

constexpr std::uint32_t kIndexMask   = (1U << kIndexBits) - 1U;
constexpr std::uint32_t kTypeMask    = (1U << kTypeBits) - 1U;
constexpr std::uint32_t kVersionMask = (1U << kVersionBits) - 1U;

template<typename T>
[[nodiscard]] auto GetUnderlying(std::uint32_t t_id, std::uint32_t t_shift, std::uint32_t t_mask) -> T
{
    return static_cast<T>((t_id >> t_shift) & t_mask);
}

}// namespace

auto RenderGraphResourceHandle::Index() const -> std::size_t
{
    return GetUnderlying<std::size_t>(id, kIndexShift, kIndexMask);
}

auto RenderGraphResourceHandle::Index16() const -> std::uint16_t
{
    return GetUnderlying<std::uint16_t>(id, kIndexShift, kIndexMask);
}

auto RenderGraphResourceHandle::Type() const -> RenderGraphResourceType
{
    return GetUnderlying<RenderGraphResourceType>(id, kTypeShift, kTypeMask);
}

auto RenderGraphResourceHandle::Version() const -> std::uint16_t
{
    return GetUnderlying<std::uint16_t>(id, kVersionShift, kVersionMask);
}

auto RenderGraphResourceHandle::IsValid() const -> bool
{
    return id != 0U;
}

auto RenderGraphResourceHandle::Create(std::uint16_t t_index, RenderGraphResourceType t_type, std::uint16_t t_version)
    -> RenderGraphResourceHandle
{
    // clang-format off
    std::uint32_t const handle_id = (static_cast<uint32_t>(t_index) << kIndexShift) |
                              (static_cast<uint32_t>(t_type) << kTypeShift) |
                              (static_cast<uint32_t>(t_version) << kVersionShift);
    // clang-format on
    return {handle_id};
}

auto RenderGraphResourceHandle::Invalid() -> RenderGraphResourceHandle
{
    return {0U};
}

}// namespace polos::rendering