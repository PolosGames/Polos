///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "helpers.hpp"

#include <polos/communication/engine_terminate.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/key_press.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/base_live_layer.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/ecs/scene_manager.hpp>
#include <polos/gui/gui.hpp>
#include <polos/rendering/rendering_api.hpp>

#include <GLFW/glfw3.h>

#include <cstdint>
#include <format>

namespace sandbox
{

class SandboxApp final : public polos::core::BaseLiveLayer
{
public:
    auto               Create() -> void override;
    [[nodiscard]] auto Name() const -> char const* override;
private:
    void drawLightEditor(polos::ecs::Scene& tScene, polos::ecs::EntityHandle tLight);
    auto drawFpsOverlay(float tDeltaTime) -> void;

    float         mFps{0.0F};
    float         mFpsSampleTime{0.0F};
    std::uint32_t mFpsSampleFrames{0U};
};

auto SandboxApp::Create() -> void
{
    using namespace polos;

    auto* scene = ecs::SceneManager::Get().CreateScene();

    CreateEntityFromModel("model.suzanne"_sid, scene);
    // auto*      transform = scene->GetMutableComponent<ecs::TransformComponent>(entity);

    auto const light = CreateLightSource(scene);
    communication::Subscribe<communication::RenderUpdate>([this, scene, light](communication::RenderUpdate& tEvent) {
        drawFpsOverlay(tEvent.deltaTime);
        drawLightEditor(*scene, light);
    });
    communication::Subscribe<communication::KeyPress>([](communication::KeyPress& tEvent) {
        if (tEvent.key == GLFW_KEY_ESCAPE)
        {
            communication::DispatchNow<communication::EngineTerminate>();
        }
    });
}

auto SandboxApp::Name() const -> char const*
{
    return "SandboxApp";
}

void SandboxApp::drawLightEditor(polos::ecs::Scene& tScene, polos::ecs::EntityHandle const tLight)
{
    namespace ecs = polos::ecs;
    namespace gui = polos::gui;

    if (gui::Begin("Light Source"))
    {
        auto const& light = *tScene.GetComponent<ecs::LightComponent>(tLight);
        gui::Color  color{.r = light.color.r, .g = light.color.g, .b = light.color.b};
        float       strength = light.ambientFactor;

        bool const colorChanged    = gui::ColorEditRgb("Color", color);
        bool const strengthChanged = gui::SliderFloat("Ambient strength", strength, 0.0F, 2.0F);
        if (colorChanged || strengthChanged)
        {
            auto& editedLight         = *tScene.GetMutableComponent<ecs::LightComponent>(tLight);
            editedLight.color         = {color.r, color.g, color.b};
            editedLight.ambientFactor = strength;
        }
    }
    gui::End();
}

auto SandboxApp::drawFpsOverlay(float tDeltaTime) -> void
{
    namespace gui = polos::gui;
    using Flags   = gui::WindowFlags;

    constexpr float samplePeriod{0.25F};
    if (tDeltaTime > 0.0F)
    {
        mFpsSampleTime += tDeltaTime;
        ++mFpsSampleFrames;
        if (mFpsSampleTime >= samplePeriod)
        {
            mFps             = static_cast<float>(mFpsSampleFrames) / mFpsSampleTime;
            mFpsSampleTime   = 0.0F;
            mFpsSampleFrames = 0U;
        }
    }

    static constexpr auto const flags = Flags::kNoTitleBar | Flags::kNoResize | Flags::kNoMove | Flags::kAutoResize |
                                        Flags::kNoInputs | Flags::kNoDocking;
    gui::SetNextWindowPosition({.x = 8.0F, .y = 8.0F});
    gui::SetNextWindowBackgroundAlpha(0.65F);
    if (gui::Begin("FpsOverlay", nullptr, flags))
    {
        gui::Text(mFps > 0.0F ? std::format("FPS: {:.1f}", mFps) : "FPS: --");
        gui::Text(
            std::format("DrawCalls: {}", polos::rendering::RenderingApi::Get().GetLastFrameStatistics().drawCalls));
    }
    gui::End();
}
}// namespace sandbox

namespace polos
{
auto CreateApplication(int /*argc*/, char** /*argv*/) -> core::ILiveLayer*
{
    return new sandbox::SandboxApp{};
}
}// namespace polos
