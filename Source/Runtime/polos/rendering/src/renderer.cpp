//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/renderer.hpp"
#include "polos/rendering/common.hpp"

namespace polos::rendering
{

Renderer::Renderer(GLFWwindow* t_window)
    : m_window{t_window},
      m_device{std::make_unique<VulkanDevice>(m_window)}
{}

auto Renderer::Initialize() -> Result<void>
{
    auto result = m_device->Initialize();
    if (!result.has_value())
    {
        return ErrorType{result.error()};
    }

    return {};
}

auto Renderer::Shutdown() -> Result<void>
{
    return {};
}

}// namespace polos::rendering
