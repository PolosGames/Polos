///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_GLFW_WINDOW_SURFACE_HPP
#define POLOS_PLATFORM_GLFW_WINDOW_SURFACE_HPP

#include "polos/platform/i_window_surface.hpp"
#include "polos/polos_api.hpp"

struct GLFWwindow;

namespace polos::platform
{

/// Binds the renderer to glfw without the renderer knowing glfw exists.
class POLOS_API GlfwWindowSurface final : public IWindowSurface
{
public:
    explicit GlfwWindowSurface(GLFWwindow* tWindow);
    ~GlfwWindowSurface() override = default;

    [[nodiscard]] auto RequiredInstanceExtensions() const -> std::vector<char const*> override;
    [[nodiscard]] auto CreateSurface(VkInstance tInstance) const -> VkSurfaceKHR override;
    [[nodiscard]] auto GetFramebufferSize() const -> FramebufferSize override;
private:
    GLFWwindow* mWindow{nullptr};
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_GLFW_WINDOW_SURFACE_HPP
