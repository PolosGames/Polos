//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "dummy_app.hpp"

#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/logging/log_macros.hpp>
#include <polos/platform/window_manager.hpp>
#include <polos/rendering/vk_instance.hpp>

namespace dummy_app
{

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

    assert(polos::platform::WindowManager::Instance().CreateWindow(1280, 720, Name()));
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
    polos::rendering::RenderFrame();
}

}// namespace dummy_app

polos::core::ILiveLayer* GetLiveLayer(void* t_ptr)
{
    return nullptr == t_ptr ? new dummy_app::DummyApp{} : new (t_ptr) dummy_app::DummyApp{};
}
