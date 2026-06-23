//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#if defined(POLOS_WIN)
#    define NOMINMAX
#    include <windows.h>
#endif

#include "dummy_app.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
    polos::communication::Subscribe<polos::communication::EngineUpdate>(
        [this](polos::communication::EngineUpdate& t_event) {
            onEngineUpdate(t_event);
        });

    polos::communication::Subscribe<polos::communication::RenderUpdate>(
        [this](polos::communication::RenderUpdate& t_event) {
            onRenderUpdate(t_event);
        });

    polos::communication::Subscribe<polos::communication::KeyRelease>(
        [this](polos::communication::KeyRelease& t_event) {
            onKeyRelease(t_event.key);
        });

    m_obj1 = polos::rendering::RenderingApi::GetMainScene()->AddObject(glm::mat4{1.0F},
                                                                       std::make_shared<polos::rendering::Material>());

    m_obj2 = polos::rendering::RenderingApi::GetMainScene()->AddObject(glm::mat4{1.0F},
                                                                       std::make_shared<polos::rendering::Material>());
}

void DummyApp::Destroy() {}

char const* DummyApp::Name() const
{ return "DummyApp"; }

auto DummyApp::onEngineUpdate(polos::communication::EngineUpdate& /**/) -> void
{
    //LogInfo("Engine Thread Update");

    // static auto start_time = std::chrono::high_resolution_clock::now();

    // auto  current_time = std::chrono::high_resolution_clock::now();
    // float time         = std::chrono::duration<float, std::chrono::seconds ::period>(current_time - start_time).count();

    glm::mat4 model_matrix{1.0F};
    // model_matrix = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    polos::rendering::RenderingApi::GetMainScene()->GetObject(m_obj1).transform = model_matrix;

    model_matrix = glm::translate(glm::mat4{1.0F}, glm::vec3{0.0F, 0.0F, 0.5F});
    // model_matrix = glm::rotate(model_matrix, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    polos::rendering::RenderingApi::GetMainScene()->GetObject(m_obj2).transform = model_matrix;
}

auto DummyApp::onRenderUpdate(polos::communication::RenderUpdate& /**/) -> void
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
