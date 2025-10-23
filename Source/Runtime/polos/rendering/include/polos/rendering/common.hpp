//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMMON_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMMON_HPP_

#include "polos/communication/error_code.hpp"

// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <cmath>

#define CHECK_VK_SUCCESS_OR_ERR(Result, Errc) \
    if (VK_SUCCESS != Result)                 \
    {                                         \
        return polos::ErrorType{Errc};        \
    }

#define VK_SIZE_CAST(Var) static_cast<std::uint32_t>(Var)

namespace polos::rendering
{

inline constexpr std::float_t const kPolosRed{0.50980395F};
inline constexpr std::float_t const kPolosGreen{0.59607846F};
inline constexpr std::float_t const kPolosBlue{0.6431373F};

inline constexpr VkClearValue const kPolosClear{.color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0f}}};

enum class RenderGraphResourceType : uint8_t
{
    kTexture = 0U,
    // kBuffer  = 1U,
};

struct RenderGraphResourceHandle
{
    auto Index() -> std::size_t;
    auto Index16() -> std::uint16_t;
    auto Type() -> RenderGraphResourceType;
    auto Version() -> std::uint16_t;
    auto IsValid() -> bool;

    static auto Create(std::uint16_t t_index, RenderGraphResourceType t_type, std::uint16_t t_version)
        -> RenderGraphResourceHandle;

    static auto Invalid() -> RenderGraphResourceHandle;

    operator std::uint32_t()
    {
        return id;
    }

    std::uint32_t id{0U};
};

inline bool operator==(RenderGraphResourceHandle t_lhs, RenderGraphResourceHandle t_rhs)
{
    return t_lhs.id == t_rhs.id;
}

inline bool operator!=(RenderGraphResourceHandle t_lhs, RenderGraphResourceHandle t_rhs)
{
    return operator==(t_lhs, t_rhs);
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMMON_HPP_
