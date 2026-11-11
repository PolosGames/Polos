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

namespace sandbox
{

namespace
{
static constexpr size_t const kRowCount{100U};
static constexpr size_t const kColumnCount{100U};
}// namespace

class SandboxApp : public polos::core::BaseLiveLayer
{
public:
    auto Create() -> void override
    {
        using namespace polos;

        auto* scene = ecs::SceneManager::CreateScene();

        constexpr float const kApart = 2.0F;

        for (size_t i{0U}; i < kRowCount; ++i)
        {
            for (size_t j{0U}; j < kColumnCount; ++j)
            {
                suzannes[j + (i * kColumnCount)] = CreateEntityFromModel("model.suzanne"_sid, scene);
                scene->GetMutableComponent<ecs::TransformComponent>(suzannes[j + (i * kColumnCount)])->position =
                    glm::vec3(static_cast<float>(j) * kApart, static_cast<float>(i), 0.0F);
            }
        }

        communication::Subscribe<communication::RenderUpdate>([this](communication::RenderUpdate& tEvent) {
            drawFpsOverlay(tEvent.deltaTime);
        });
        communication::Subscribe<communication::KeyPress>([](communication::KeyPress& tEvent) {
            if (tEvent.key == GLFW_KEY_ESCAPE)
            {
                communication::DispatchNow<communication::EngineTerminate>();
            }
        });
    }

    [[nodiscard]] auto Name() const -> char const* override
    {
        return "SandboxApp";
    }
private:
    auto drawFpsOverlay(float tDeltaTime) -> void
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

        static auto const flags = Flags::kNoTitleBar | Flags::kNoResize | Flags::kNoMove | Flags::kAutoResize |
                                  Flags::kNoInputs | Flags::kNoDocking;
        gui::SetNextWindowPosition({.x = 8.0F, .y = 8.0F});
        gui::SetNextWindowBackgroundAlpha(0.65F);
        if (gui::Begin("FpsOverlay", nullptr, flags))
        {
            gui::Text(mFps > 0.0F ? std::format("FPS: {:.1f}", mFps) : "FPS: --");
            gui::Text(std::format("DrawCalls: {}", polos::rendering::RenderingApi::GetLastFrameStatistics().drawCalls));
        }
        gui::End();
    }

    float         mFps{0.0F};
    float         mFpsSampleTime{0.0F};
    std::uint32_t mFpsSampleFrames{0U};

    std::array<polos::ecs::EntityHandle, kRowCount * kColumnCount> suzannes;
};
}// namespace sandbox

namespace polos
{
auto CreateApplication(int /*argc*/, char** /*argv*/) -> core::ILiveLayer*
{
    return new sandbox::SandboxApp{};
}
}// namespace polos
