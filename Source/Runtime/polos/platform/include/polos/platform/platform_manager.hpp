//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_PLATFORM_MANAGER_HPP_
#define POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_PLATFORM_MANAGER_HPP_

#include "polos/platform/module_macros.hpp"
#include "polos/rendering/i_render_context.hpp"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string_view>

struct GLFWwindow;

namespace polos::core
{
class Engine;
}// namespace polos::core

namespace polos::communication
{
struct rendering_module_reload;
}// namespace polos::communication

namespace polos::rendering
{
class RenderContext;
}// namespace polos::rendering

namespace polos::platform
{

class PLATFORM_EXPORT PlatformManager
{
public:
    PlatformManager();
    ~PlatformManager();

    PlatformManager(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)      = delete;

    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;

    static PlatformManager& Instance();

    auto CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title) -> bool;
    auto ChangeWindowTitle(std::string_view const t_title) -> void;
    auto GetMainWindow() const -> GLFWwindow*;

#if defined(HOT_RELOAD)
    auto CheckNeedModuleReload() -> bool;
#endif// HOT_RELOAD

    auto RenderingContextInstance() const -> rendering::IRenderContext&;
private:
    friend class core::Engine;

    static PlatformManager* s_instance;

    void create_render_context();
    void init_vulkan();

    void on_end_frame() const;
    void on_window_close();
    void on_engine_terminate();
    void on_module_reload(communication::rendering_module_reload& t_event);

    GLFWwindow*                                m_window{nullptr};
    std::unique_ptr<rendering::IRenderContext> m_render_context;

#if defined(HOT_RELOAD)
    std::filesystem::file_time_type       m_last_modified;
    std::chrono::steady_clock::time_point m_last_check;
#endif// HOT_RELOAD
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_PLATFORM_MANAGER_HPP_
