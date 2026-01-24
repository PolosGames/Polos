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

namespace polos::rendering::common
{

constexpr std::float_t const kPolosRed{0.50980395F};
constexpr std::float_t const kPolosGreen{0.59607846F};
constexpr std::float_t const kPolosBlue{0.6431373F};

}// namespace polos::rendering::common

#endif// POLOS_RENDERING_COMMON_HPP
