//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERING_API_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERING_API_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/shared_lib_out.hpp"

#include <memory>

namespace polos::core
{
class Engine;
};// namespace polos::core

namespace polos::rendering
{

class IRenderContext;

class RENDERING_EXPORT RenderingApi
{
public:
    explicit RenderingApi(GLFWwindow* t_window);
    ~RenderingApi();

    RenderingApi(RenderingApi const&)            = delete;
    RenderingApi(RenderingApi&&)                 = delete;
    RenderingApi& operator=(RenderingApi const&) = delete;
    RenderingApi& operator=(RenderingApi&&)      = delete;

    auto Shutdown() -> void;

    static auto BeginFrame() -> VkCommandBuffer;
    static auto EndFrame() -> void;

#if defined(HOT_RELOAD)
    static auto ReloadIfNeeded() -> bool;
    static auto DispatchReload() -> void;
#endif// HOT_RELOAD
private:
    friend class core::Engine;

    static RenderingApi* s_instance;

#if defined(HOT_RELOAD)
    bool loadRenderingImplModule();
#endif// HOT_RELOAD

    void createRenderContext();
    void initVulkan();

    GLFWwindow* m_window{nullptr};

    std::shared_ptr<IRenderContext> m_render_context;

#if defined(HOT_RELOAD)
    rendering::rendering_shared_lib_out m_rendering_module;
    bool                                m_should_reload{false};
#endif// HOT_RELOAD
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERING_API_HPP_
