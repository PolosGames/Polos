///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_PLATFORM_MANAGER_HPP
#define POLOS_PLATFORM_PLATFORM_MANAGER_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/interface/i_window_surface.hpp"

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
struct RenderingModuleReload;
}// namespace polos::communication

namespace polos::platform
{

class GlfwWindowSurface;

class POLOS_API PlatformManager
{
public:
    PlatformManager();
    ~PlatformManager();

    PlatformManager(PlatformManager const&) = delete;
    PlatformManager(PlatformManager&&)      = delete;

    PlatformManager& operator=(PlatformManager const&) = delete;
    PlatformManager& operator=(PlatformManager&&)      = delete;

    static auto Instance() -> PlatformManager&;

    auto               CreateNewWindow(std::int32_t tWidth, std::int32_t tHeight, std::string_view tTitle) -> bool;
    auto               ChangeWindowTitle(std::string_view tTitle) -> void;
    [[nodiscard]] auto GetMainWindow() const -> GLFWwindow*;
    [[nodiscard]] auto GetWindowSurface() const -> rendering::IWindowSurface&;
private:
    friend class core::Engine;

    static PlatformManager* sInstance;

    void on_end_frame() const;
    GLFWwindow*                        mWindow{nullptr};
    std::unique_ptr<GlfwWindowSurface> mWindowSurface;
    bool                               mGlfwInitialized{false};
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_PLATFORM_MANAGER_HPP
