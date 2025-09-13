//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERER_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERER_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/device.hpp"

#include <memory>

namespace polos::rendering
{

class Renderer
{
public:
    explicit Renderer(GLFWwindow* t_window);
    ~Renderer();
    Renderer(Renderer&&)            = delete;
    Renderer(Renderer&)             = delete;
    Renderer& operator=(Renderer&&) = delete;
    Renderer& operator=(Renderer&)  = delete;

    auto Initialize() -> Result<void>;
    auto Shutdown() -> Result<void>;
private:
    GLFWwindow* m_window;

    std::unique_ptr<VulkanDevice> m_device;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERER_HPP_
