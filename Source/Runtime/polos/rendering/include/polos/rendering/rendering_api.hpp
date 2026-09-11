///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDERING_API_HPP
#define POLOS_RENDERING_RENDERING_API_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/i_window_surface.hpp"
#include "polos/rendering/scene.hpp"
#include "polos/rendering/shared_lib_out.hpp"

#include <memory>

namespace polos::core
{
class Engine;
};// namespace polos::core

namespace polos::rendering
{

class IRenderContext;

class POLOS_API RenderingApi
{
public:
    explicit RenderingApi(IWindowSurface& t_surface);
    ~RenderingApi();

    RenderingApi(RenderingApi const&)            = delete;
    RenderingApi(RenderingApi&&)                 = delete;
    RenderingApi& operator=(RenderingApi const&) = delete;
    RenderingApi& operator=(RenderingApi&&)      = delete;

    auto Shutdown() -> void;

    static auto BeginFrame() -> VkCommandBuffer;
    static auto EndFrame() -> void;
    static auto GetMainScene() -> std::shared_ptr<Scene>;
private:
    friend class core::Engine;

    static RenderingApi* s_instance;

    void createRenderContext();
    void initVulkan();

    IWindowSurface* m_window_surface{nullptr};

    std::shared_ptr<IRenderContext> m_render_context;
    std::shared_ptr<Scene>          m_main_scene;

#if defined(HOT_RELOAD)
public:
    static auto ReloadIfNeeded() -> bool;
    static auto DispatchReload() -> void;
private:
    bool loadRenderingImplModule();

    rendering::RenderingSharedLibOut m_rendering_module;
    bool                                m_should_reload{false};
#endif// HOT_RELOAD
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDERING_API_HPP
