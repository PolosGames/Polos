///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "helpers.hpp"
#include "polos/ecs/view.hpp"

#include <polos/communication/engine_terminate.hpp>
#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/key_press.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/base_live_layer.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/ecs/scene_manager.hpp>
#include <polos/gui/gui.hpp>
#include <polos/logging/log_macros.hpp>
#include <polos/rendering/rendering_api.hpp>

#include <GLFW/glfw3.h>

#include <chrono>
#include <cstdint>
#include <format>
#include <future>
#include <string_view>
#include <utility>

namespace sandbox
{

namespace
{
constexpr std::string_view kScenePath{"Sandbox.yaml"};
}

class SandboxApp final : public polos::core::BaseLiveLayer
{
public:
    auto               Create() -> void override;
    [[nodiscard]] auto Name() const -> char const* override;
private:
    void drawLightEditor(polos::ecs::Scene* tScene);
    void drawFpsOverlay(float tDeltaTime);
    void drawSceneControl();

    float             mFps{0.0F};
    float             mFpsSampleTime{0.0F};
    std::uint32_t     mFpsSampleFrames{0U};
    std::future<void> mSceneSave;
    bool              mReloadScene{false};
};

auto SandboxApp::Create() -> void
{
    using namespace polos;

    auto& scenes = ecs::SceneManager::Get();
    if (scenes.LoadScene(kScenePath) == nullptr && scenes.GetCurrentScene() == nullptr)
    {
        scenes.CreateScene(kScenePath);
    }

    communication::Subscribe<communication::EngineUpdate>([this](communication::EngineUpdate&) {
        if (std::exchange(mReloadScene, false))
        {
            ecs::SceneManager::Get().LoadScene(kScenePath);
        }
    });
    communication::Subscribe<communication::RenderUpdate>([this](communication::RenderUpdate& tEvent) {
        drawFpsOverlay(tEvent.deltaTime);
        drawLightEditor(ecs::SceneManager::Get().GetCurrentScene());
        drawSceneControl();
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

void SandboxApp::drawLightEditor(polos::ecs::Scene* tScene)
{
    namespace ecs = polos::ecs;
    namespace gui = polos::gui;

    if (tScene == nullptr)
    {
        return;
    }

    auto lights = ecs::View<ecs::EntityHandle, ecs::Read<ecs::LightComponent>>{tScene};
    auto first  = lights.begin();
    if (first == lights.end())
    {
        return;
    }

    if (gui::Begin("Light Source"))
    {
        auto [entity, light] = *first;
        gui::Color color{.r = light->color.r, .g = light->color.g, .b = light->color.b};
        float      strength        = light->ambientFactor;
        bool const colorChanged    = gui::ColorEditRgb("Color", color);
        bool const strengthChanged = gui::SliderFloat("Ambient strength", strength, 0.0F, 2.0F);
        if (colorChanged || strengthChanged)
        {
            auto& editedLight         = *tScene->GetMutableComponent<ecs::LightComponent>(entity);
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

void SandboxApp::drawSceneControl()
{
    namespace ecs = polos::ecs;
    namespace gui = polos::gui;

    if (gui::Begin("Scene Control"))
    {
        bool const saveRequested = gui::Button("Save Current Scene");
        bool const savePending =
            mSceneSave.valid() && mSceneSave.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
        if (saveRequested && !savePending)
        {
            auto snapshot = ecs::SceneManager::Get().TakeCurrentSceneSnapshot();
            if (!snapshot)
            {
                LogError("{}", snapshot.error().Message());
            }
            else
            {
                mSceneSave = std::async(std::launch::async, [snapshot = std::move(*snapshot)] {
                    if (auto result = ecs::SceneManager::SaveSceneSnapshot(*snapshot); !result)
                    {
                        LogError("{}", result.error().Message());
                    }
                });
            }
        }

        if (gui::Button("Load Scene") && !savePending && !saveRequested)
        {
            mReloadScene = true;
        }
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
