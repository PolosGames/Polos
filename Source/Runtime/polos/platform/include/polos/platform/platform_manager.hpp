///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_PLATFORM_MANAGER_HPP
#define POLOS_PLATFORM_PLATFORM_MANAGER_HPP

#include "polos/platform/module_macros.hpp"
#include "polos/rendering/i_render_context.hpp"
#if defined(HOT_RELOAD)
#    include "polos/rendering/shared_lib_out.hpp"
#endif// HOT_RELOAD

#include <cstdint>
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
    ~PlatformManager() = default;

    PlatformManager(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)      = delete;

    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;

    static auto Instance() -> PlatformManager&;

    auto               CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title) -> bool;
    auto               ChangeWindowTitle(std::string_view t_title) -> void;
    [[nodiscard]] auto GetMainWindow() const -> GLFWwindow*;
private:
    friend class core::Engine;

    static PlatformManager* s_instance;

    void on_end_frame() const;
    void on_engine_terminate();

    GLFWwindow* m_window{nullptr};
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_PLATFORM_MANAGER_HPP
