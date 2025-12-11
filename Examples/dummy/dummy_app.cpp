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
#include <polos/rendering/material.hpp>
#include <polos/rendering/rendering_api.hpp>
#include <polos/rendering/scene.hpp>

#include <memory>

namespace dummy_app
{

DummyApp::DummyApp()  = default;
DummyApp::~DummyApp() = default;

void DummyApp::Create()
{
    polos::communication::Subscribe<polos::communication::engine_update>(
        [this](polos::communication::engine_update& t_event) {
            onEngineUpdate(t_event);
        });

    polos::communication::Subscribe<polos::communication::render_update>(
        [this](polos::communication::render_update& t_event) {
            onRenderUpdate(t_event);
        });

    polos::communication::Subscribe<polos::communication::key_release>(
        [this](polos::communication::key_release& t_event) {
            onKeyRelease(t_event.key);
        });

    polos::rendering::RenderingApi::GetMainScene()->AddObject(
        glm::mat4{1.0F},
        std::make_shared<polos::rendering::material>());
}

void DummyApp::Destroy() {}

char const* DummyApp::Name() const
{
    return "DummyApp";
}

auto DummyApp::onEngineUpdate(polos::communication::engine_update& /**/) -> void
{
    //LogInfo("Engine Thread Update");
}

auto DummyApp::onRenderUpdate(polos::communication::render_update& /**/) -> void
{
    //LogInfo("Render Thread Update");
}

auto DummyApp::onKeyRelease(std::int32_t /*t_key*/) -> void {}

}// namespace dummy_app

namespace polos
{

auto CreateApplication(int /*argc*/, char** /*argv*/) -> polos::core::ILiveLayer*
{
    return new dummy_app::DummyApp{};//NOLINT(cppcoreguidelines-owning-memory)
}

}// namespace polos
