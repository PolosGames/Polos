///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_
#define POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_

#include "polos/platform/module_macros.hpp"

#if defined(HOT_RELOAD)
#    include "polos/rendering/shared_lib_out.hpp"
#endif// HOT_RELOAD

#include <cstdint>
#include <memory>
#include <string_view>

struct GLFWwindow;

namespace polos::rendering
{
class RenderContext;
}// namespace polos::rendering

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

    bool        CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title);
    void        ChangeWindowTitle(std::string_view const t_title);
    GLFWwindow* GetRawWindow() const;

#if defined(HOT_RELOAD)
    /// @brief Must be called before CreateNewWindow
    void UpdateRenderingModule(rendering::rendering_shared_lib_out& t_dll_out);
#endif// HOT_RELOAD
private:
    WindowManager();

    void init_vulkan();

    void on_end_frame() const;
    void on_window_close();
    void on_engine_terminate();

    GLFWwindow*                               m_window{nullptr};
    std::unique_ptr<rendering::RenderContext> m_rendering_context;
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_