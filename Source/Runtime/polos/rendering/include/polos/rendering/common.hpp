///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_COMMON_HPP
#define POLOS_RENDERING_COMMON_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/module_macros.hpp"

#define CHECK_VK_SUCCESS_OR_ERR(Result, Errc) \
    if (VK_SUCCESS != (Result))               \
    {                                         \
        return polos::ErrorType{(Errc)};      \
    }// NOLINT

#define VK_SIZE_CAST(Var) static_cast<std::uint32_t>(Var)// NOLINT

namespace polos::rendering
{

enum class RenderGraphResourceType : uint8_t
{
    kTexture,
    kBuffer,
};

struct RENDERING_EXPORT RenderGraphResourceHandle
{
    [[nodiscard]] auto Index() const -> std::size_t;
    [[nodiscard]] auto Index16() const -> std::uint16_t;
    [[nodiscard]] auto Type() const -> RenderGraphResourceType;
    [[nodiscard]] auto Version() const -> std::uint16_t;
    [[nodiscard]] auto IsValid() const -> bool;

    static auto Create(std::uint16_t t_index, RenderGraphResourceType t_type, std::uint16_t t_version)
        -> RenderGraphResourceHandle;

    static auto Invalid() -> RenderGraphResourceHandle;

    explicit operator std::uint32_t() const
    {
        return id;
    }

    std::uint32_t id{0U};
};

inline auto operator==(RenderGraphResourceHandle t_lhs, RenderGraphResourceHandle t_rhs) -> bool
{
    return t_lhs.id == t_rhs.id;
}

inline auto operator!=(RenderGraphResourceHandle t_lhs, RenderGraphResourceHandle t_rhs) -> bool
{
    return !operator==(t_lhs, t_rhs);
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_COMMON_HPP
