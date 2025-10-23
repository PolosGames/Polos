//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/common.hpp"

namespace polos::rendering
{

namespace
{
static constexpr std::uint32_t kIndexBits{16U};
static constexpr std::uint32_t kTypeBits{4U};
static constexpr std::uint32_t kVersionBits{12U};

static constexpr std::uint32_t kIndexShift{0U};
static constexpr std::uint32_t kTypeShift    = kIndexBits;
static constexpr std::uint32_t kVersionShift = kIndexBits + kTypeBits;

static constexpr std::uint32_t kIndexMask   = (1 << kIndexBits) - 1;
static constexpr std::uint32_t kTypeMask    = (1 << kTypeBits) - 1;
static constexpr std::uint32_t kVersionMask = (1 << kVersionBits) - 1;
}// namespace

template<typename T>
T GetUnderlying(std::uint32_t t_id, std::uint32_t t_shift, std::uint32_t t_mask)
{
    return static_cast<T>((t_id >> t_shift) & t_mask);
}

auto RenderGraphResourceHandle::Index() -> std::size_t
{
    return GetUnderlying<std::size_t>(id, kIndexShift, kIndexMask);
}

auto RenderGraphResourceHandle::Index16() -> std::uint16_t
{
    return GetUnderlying<std::uint16_t>(id, kIndexShift, kIndexMask);
}

auto RenderGraphResourceHandle::Type() -> RenderGraphResourceType
{
    return GetUnderlying<RenderGraphResourceType>(id, kTypeShift, kTypeMask);
}

auto RenderGraphResourceHandle::Version() -> std::uint16_t
{
    return GetUnderlying<std::uint16_t>(id, kVersionShift, kVersionMask);
}

auto RenderGraphResourceHandle::IsValid() -> bool
{
    return id != 0U;
}

auto RenderGraphResourceHandle::Create(std::uint16_t t_index, RenderGraphResourceType t_type, std::uint16_t t_version)
    -> RenderGraphResourceHandle
{
    // clang-format off
    std::uint32_t const id = (static_cast<uint32_t>(t_index) << kIndexShift) |
                       (static_cast<uint32_t>(t_type) << kTypeShift) |
                       (static_cast<uint32_t>(t_version) << kVersionShift);
    // clang-format on
    return {id};
}

auto RenderGraphResourceHandle::Invalid() -> RenderGraphResourceHandle
{
    return {0U};
}

}// namespace polos::rendering