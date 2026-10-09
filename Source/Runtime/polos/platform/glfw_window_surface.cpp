///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/glfw_window_surface.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/utils/assert.hpp"

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

    PL_ASSERT(extensions != nullptr, "GLFW could not provide the required Vulkan instance extensions.");

    return {extensions, std::next(extensions, static_cast<std::ptrdiff_t>(count))};
}

auto GlfwWindowSurface::CreateSurface(VkInstance tInstance) const -> VkSurfaceKHR
{
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    if (VK_SUCCESS != glfwCreateWindowSurface(tInstance, mWindow, nullptr, &surface))
    {
        LogError("Failed to create window surface.");
        return VK_NULL_HANDLE;
    }

    return surface;
}

auto GlfwWindowSurface::GetFramebufferSize() const -> FramebufferSize
{
    std::int32_t width{0};
    std::int32_t height{0};
    glfwGetFramebufferSize(mWindow, &width, &height);

    return {.width = static_cast<std::uint32_t>(width), .height = static_cast<std::uint32_t>(height)};
}

}// namespace polos::platform
