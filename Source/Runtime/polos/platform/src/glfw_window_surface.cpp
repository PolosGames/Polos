///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "glfw_window_surface.hpp"

#include "polos/rendering/rendering_error_domain.hpp"

#include <GLFW/glfw3.h>

#include <iterator>

namespace polos::platform
{

GlfwWindowSurface::GlfwWindowSurface(GLFWwindow* tWindow)
    : mWindow{tWindow}
{}

auto GlfwWindowSurface::RequiredInstanceExtensions() const -> std::vector<char const*>
{
    std::uint32_t count{0U};
    char const**  extensions = glfwGetRequiredInstanceExtensions(&count);

    if (nullptr == extensions)
    {
        return {};
    }

    return {extensions, std::next(extensions, static_cast<std::ptrdiff_t>(count))};
}

auto GlfwWindowSurface::CreateSurface(VkInstance tInstance) const -> Result<VkSurfaceKHR>
{
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    if (VK_SUCCESS != glfwCreateWindowSurface(tInstance, mWindow, nullptr, &surface))
    {
        return ErrorType{rendering::RenderingErrc::kFailedCreateSurface};
    }

    return surface;
}

auto GlfwWindowSurface::GetFramebufferSize() const -> rendering::FramebufferSize
{
    std::int32_t width{0};
    std::int32_t height{0};
    glfwGetFramebufferSize(mWindow, &width, &height);

    return {.width = static_cast<std::uint32_t>(width), .height = static_cast<std::uint32_t>(height)};
}

}// namespace polos::platform
