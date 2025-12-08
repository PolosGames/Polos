//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#if defined(POLOS_WIN)
#    define NOMINMAX
#    include <windows.h>
#endif

#include "dummy_app.hpp"

#include <polos/communication/engine_terminate.hpp>
#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/communication/rendering_module_reload.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/logging/log_macros.hpp>

namespace dummy_app
{

DummyApp::DummyApp()  = default;
DummyApp::~DummyApp() = default;

void DummyApp::Create()
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
}

void DummyApp::Destroy() {}

char const* DummyApp::Name() const
{
    return "DummyApp";
}

void DummyApp::on_engine_update(polos::communication::engine_update& /**/)
{
    //LogInfo("Engine Thread Update");
}

void DummyApp::on_render_update(polos::communication::render_update& /**/) {}

void DummyApp::on_key_release(polos::communication::key_release /**/) {}

}// namespace dummy_app

namespace polos
{

polos::core::ILiveLayer* CreateApplication(int /*argc*/, char** /*argv*/)
{
    return new dummy_app::DummyApp{};
}

}// namespace polos
