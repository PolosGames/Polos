///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDERING_API_HPP
#define POLOS_RENDERING_RENDERING_API_HPP

#include "polos/platform/i_window_surface.hpp"
#include "polos/polos_api.hpp"
#include "polos/rendering/frame_statistics.hpp"

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
    [[nodiscard]] static auto Get() -> RenderingApi&;

    explicit RenderingApi(platform::IWindowSurface& tSurface);
    ~RenderingApi();

    RenderingApi(RenderingApi const&)            = delete;
    RenderingApi(RenderingApi&&)                 = delete;
    RenderingApi& operator=(RenderingApi const&) = delete;
    RenderingApi& operator=(RenderingApi&&)      = delete;

    auto Shutdown() -> void;

    auto BeginFrame() -> VkCommandBuffer;
    auto EndFrame() -> void;

    auto GetLastFrameStatistics() -> FrameStatistics const&;
private:
    friend class core::Engine;

    static RenderingApi* sInstance;

    void createRenderContext();
    void initVulkan();

    platform::IWindowSurface* mWindowSurface{nullptr};

    FrameStatistics                 lastStatistics{};
    std::shared_ptr<IRenderContext> mRenderContext;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDERING_API_HPP
