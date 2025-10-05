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

using RendererResourceHandle = std::uint32_t;

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMMON_HPP_
