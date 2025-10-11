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

#define CHECK_VK_SUCCESS_OR_ERR(Result, Errc) \
    if (VK_SUCCESS != Result)                 \
    {                                         \
        return polos::ErrorType{Errc};        \
    }

namespace polos::rendering
{

enum class RendererResourceType : uint8_t
{
    kTexture = 0U,
    // kBuffer  = 1U,
};

struct RenderGraphResourceHandle
{
    auto Index() -> std::size_t;
    auto Index16() -> std::uint16_t;
    auto Type() -> RendererResourceType;
    auto Version() -> std::uint16_t;
    auto IsValid() -> bool;

    static auto Create(std::uint16_t t_index, RendererResourceType t_type, std::uint16_t t_version)
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
