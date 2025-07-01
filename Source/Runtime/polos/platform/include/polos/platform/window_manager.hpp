///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_
#define POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_

#include "polos/platform/module_macros.hpp"

#include <GLFW/glfw3.h>

#include <string_view>
#include <vector>

namespace polos::communication
{

struct engine_update;
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

    bool        CreateWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title);
    GLFWwindow* GetRawWindow() const;
private:
    WindowManager();

    void OnUpdate(communication::engine_update&) const noexcept;
    void OnWindowClose(communication::window_close&) const noexcept;

    GLFWwindow* m_window{nullptr};
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_