//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/i_render_context.hpp"
#include "polos/rendering/rendering_api.hpp"

#include <GLFW/glfw3.h>

namespace polos::rendering
{

RenderingApi* RenderingApi::s_instance{nullptr};

RenderingApi::RenderingApi(GLFWwindow* t_window)
    : m_window{t_window}
{
    createRenderContext();

    m_main_scene = std::make_shared<Scene>();
}

RenderingApi::~RenderingApi() = default;

auto RenderingApi::Shutdown() -> void
{
    std::ignore = m_render_context->Shutdown();

#if defined(HOT_RELOAD)
    UnloadRenderingModule(m_rendering_module);
#endif// HOT_RELOAD
}

auto RenderingApi::BeginFrame() -> VkCommandBuffer
{
    if (nullptr == s_instance->m_render_context)
    {
        return VK_NULL_HANDLE;
    }
    return s_instance->m_render_context->BeginFrame();
}

auto RenderingApi::EndFrame() -> void
{
    if (nullptr != s_instance->m_render_context)
    {
        s_instance->m_render_context->EndFrame();
    }
}

auto RenderingApi::GetMainScene() -> std::shared_ptr<Scene>
{
    return s_instance->m_main_scene;
}

#if defined(HOT_RELOAD)
auto RenderingApi::ReloadIfNeeded() -> bool
{
    if (!s_instance->m_should_reload)
    {
        return false;
    }

    s_instance->m_should_reload = false;
    s_instance->createRenderContext();

    LogInfo("Reloaded Render module.");

    return true;
}

auto RenderingApi::DispatchReload() -> void
{
    s_instance->m_should_reload = true;
}

auto RenderingApi::loadRenderingImplModule() -> bool
{
    LogInfo("Loading Rendering Module");

    UnloadRenderingModule(m_rendering_module);
    if (!LoadRenderingModule(m_rendering_module))
    {
        communication::DispatchNow<communication::engine_terminate>();
        return false;
    }

    return true;
}
#endif// HOT_RELOAD

void RenderingApi::createRenderContext()
{
    if (nullptr != m_render_context)
    {
        std::ignore = m_render_context->Shutdown();
        m_render_context.reset();
    }

#if defined(HOT_RELOAD)
    loadRenderingImplModule();
    m_render_context = std::unique_ptr<IRenderContext>(m_rendering_module.CreateRenderContext());
#else
    m_render_context = std::unique_ptr<IRenderContext>(CreateRenderContext());
#endif// HOT_RELOAD

    auto result = m_render_context->Initialize(m_window);
    if (!result.has_value())
    {
        LogCritical("RenderContext could not be initialized! {}", result.error().Message());
        communication::DispatchNow<communication::engine_terminate>();
        return;
    }
}

}// namespace polos::rendering
