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

struct module_reload;

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
private:
    WindowManager();

    void create_render_context();
    void init_vulkan();

    void on_end_frame() const;
    void on_window_close();
    void on_engine_terminate();
    void on_module_reload(communication::module_reload& t_event);

    GLFWwindow* m_window{nullptr};

    struct render_context_deleter
    {
        void operator()(rendering::RenderContext* t_ptr);

        WindowManager* wm;
    };

    std::unique_ptr<rendering::RenderContext, render_context_deleter> m_rendering_context;
    render_context_deleter                                            m_deleter{this};

#if defined(HOT_RELOAD)
    rendering::rendering_shared_lib_out* m_rendering_module{nullptr};
#endif
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_