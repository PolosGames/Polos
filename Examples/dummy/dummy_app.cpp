//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#if defined(POLOS_WIN)
#    define NOMINMAX
#    include <windows.h>
#endif

#include "dummy_app.hpp"

#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/logging/log_macros.hpp>
#include <polos/platform/window_manager.hpp>
#if defined(HOT_RELOAD)
#    include <polos/rendering/shared_lib_out.hpp>
#endif// HOT_RELOAD
#include <polos/rendering/vk_instance.hpp>

namespace dummy_app
{

#if defined(HOT_RELOAD)
namespace
{
polos::rendering::rendering_shared_lib_out rendering_dll;
}// namespace
#endif

DummyApp::DummyApp()
{
    polos::communication::Subscribe<polos::communication::engine_update>(
        [this](polos::communication::engine_update& t_event) {
            on_engine_update(t_event);
        });

    polos::communication::Subscribe<polos::communication::render_update>(
        [this](polos::communication::render_update& t_event) {
            on_render_update(t_event);
        });

    polos::communication::Subscribe<polos::communication::key_release>(
        [this](polos::communication::key_release& t_event) {
            on_key_release(t_event);
        });

    auto& win_inst = polos::platform::WindowManager::Instance();
#if defined(HOT_RELOAD)
    polos::rendering::LoadRenderingModule(rendering_dll);
    win_inst.UpdateRenderingModule(rendering_dll);
#endif// HOT_RELOAD

    assert(win_inst.CreateNewWindow(1280, 720, Name()));
}

DummyApp::~DummyApp() {}

char const* DummyApp::Name() const
{
    return "DummyApp";
}

void DummyApp::on_engine_update(polos::communication::engine_update&)
{
    //LogInfo("Engine Thread Update");
}

void DummyApp::on_render_update(polos::communication::render_update&)
{
#if defined(HOT_RELOAD)
    if (nullptr != rendering_dll.render_frame_func && !m_unload_in_progress)
    {
        rendering_dll.render_frame_func();
    }
#else
    polos::rendering::RenderFrame();
#endif
}

void DummyApp::on_key_release(polos::communication::key_release t_event)
{
#if defined(HOT_RELOAD)
    if (t_event.key == GLFW_KEY_R)
    {
        m_unload_in_progress = true;
        rendering_dll.terminate_vulkan_func();
        polos::rendering::UnloadRenderingModule(rendering_dll);
    }

    if (t_event.key == GLFW_KEY_S)
    {
        if (polos::rendering::LoadRenderingModule(rendering_dll))
        {
            polos::platform::WindowManager::Instance().UpdateRenderingModule(rendering_dll);
            m_unload_in_progress = false;
        }
    }
#endif// HOT_RELOAD
}


}// namespace dummy_app

polos::core::ILiveLayer* GetLiveLayer(void* t_ptr)
{
    return nullptr == t_ptr ? new dummy_app::DummyApp{} : new (t_ptr) dummy_app::DummyApp{};
}
