///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_
#define POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_

#include "polos/platform/module_macros.hpp"

#include "polos/rendering/dll_out.hpp"
#include "polos/rendering/vulkan_state.hpp"

#include <cstdint>
#include <string_view>

struct GLFWwindow;

namespace polos::communication
{

struct end_frame;
struct window_close;

}// namespace polos::communication

namespace polos::platform
{

class PLATFORM_EXPORT WindowManager
{
public:
    ~WindowManager();

    WindowManager(WindowManager const&) = delete;
    WindowManager(WindowManager&&)      = delete;

    WindowManager& operator=(WindowManager const&) = delete;
    WindowManager& operator=(WindowManager&&)      = delete;

    static WindowManager& Instance();
    bool                  CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title);
    GLFWwindow*           GetRawWindow() const;

#ifndef NDEBUG
    /// @brief Must be called before CreateNewWindow
    void UpdateRenderingModule(rendering::rendering_dll_out& t_dll_out);
#endif// NDEBUG
private:
    WindowManager();

    void init_vulkan();

    void on_end_frame() const;
    void on_window_close();
    void on_engine_terminate();

    GLFWwindow* m_window{nullptr};

    rendering::VulkanState m_vulkan_state;
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_