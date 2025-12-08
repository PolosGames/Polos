//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/rendering_api.hpp"

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/i_render_context.hpp"

#include <GLFW/glfw3.h>

namespace polos::rendering
{

RenderingApi* RenderingApi::s_instance{nullptr};

RenderingApi::RenderingApi(GLFWwindow* t_window)
    : m_window{t_window}
{
    createRenderContext();
}

RenderingApi::~RenderingApi() = default;

auto RenderingApi::Shutdown() -> void
{
    std::ignore = m_render_context->Shutdown();
}

auto RenderingApi::BeginFrame() -> VkCommandBuffer
{
    return s_instance->m_render_context->BeginFrame();
}

auto RenderingApi::EndFrame() -> void
{
    s_instance->m_render_context->EndFrame();
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

    rendering::UnloadRenderingModule(m_rendering_module);
    if (!rendering::LoadRenderingModule(m_rendering_module))
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
    m_render_context = std::unique_ptr<rendering::IRenderContext>(m_rendering_module.CreateRenderContext());
#else
    m_render_context = std::unique_ptr<rendering::IRenderContext>(CreateRenderContext());
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
