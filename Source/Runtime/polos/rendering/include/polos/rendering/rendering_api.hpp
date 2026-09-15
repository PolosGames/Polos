///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDERING_API_HPP
#define POLOS_RENDERING_RENDERING_API_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/interface/i_window_surface.hpp"
#include "polos/rendering/scene/scene.hpp"

#include <memory>

namespace polos::core
{
class Engine;
};// namespace polos::core

namespace polos::rendering
{

class IRenderContext;
struct RenderingSharedLibOut;

class POLOS_API RenderingApi
{
public:
    explicit RenderingApi(IWindowSurface& tSurface);
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

    static RenderingApi* sInstance;

    void createRenderContext();
    void initVulkan();

    IWindowSurface* mWindowSurface{nullptr};

    std::shared_ptr<IRenderContext> mRenderContext;
    std::shared_ptr<Scene>          mMainScene;

#if defined(HOT_RELOAD)
public:
    static auto ReloadIfNeeded() -> bool;
    static auto DispatchReload() -> void;
private:
    bool loadRenderingImplModule();

    std::unique_ptr<RenderingSharedLibOut> mRenderingModule;
    bool                                   mShouldReload{false};
#endif// HOT_RELOAD
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDERING_API_HPP
