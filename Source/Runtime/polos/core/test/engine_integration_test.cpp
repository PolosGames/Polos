#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/render_update.hpp"
#include "polos/core/engine.hpp"
#include "polos/core/i_live_layer.hpp"
#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/quad_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/scene_manager.hpp"
#include "polos/gui/gui.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/resource/resource_manager.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace polos::core
{
namespace
{
constexpr unsigned kFrameCount{5U};

struct SmokeState
{
    unsigned                      updates{0U};
    unsigned                      renderUpdates{0U};
    unsigned                      completedFrames{0U};
    unsigned                      guiFrames{0U};
    bool                          windowCreated{false};
    bool                          shadersLoaded{false};
    std::vector<std::string_view> lifecycle;
};

class SmokeApplication final : public ILiveLayer
{
public:
    explicit SmokeApplication(SmokeState& tState)
        : mState{tState}
    {}

    ~SmokeApplication() override
    {
        mState.lifecycle.push_back("destruct");
    }

    auto Name() const -> char const* override
    {
        return "EngineIntegrationTest";
    }

    auto Create() -> void override
    {
        mState.lifecycle.push_back("create");
        auto&      platform    = platform::PlatformManager::Instance();
        auto const framebuffer = platform.GetWindowSurface().GetFramebufferSize();
        mState.windowCreated   = platform.GetMainWindow() != nullptr && framebuffer.width > 0 && framebuffer.height > 0;

        auto& resources      = resource::ResourceManager::Instance();
        mState.shadersLoaded = resources.GetFile("shader.quad.vertex"_sid) != nullptr &&
                               resources.GetFile("shader.quad.fragment"_sid) != nullptr;

        auto*                            scene  = ecs::SceneManager::CreateScene();
        auto const                       camera = scene->NewEntity(scene->sceneRoot, "SmokeCamera"_sid);
        ecs::CameraComponent const       cameraSettings{.nearZ = 0.1F, .farZ = 100.0F, .fov = 60.0F};
        ecs::TransformComponent const    cameraTransform{.position = {0.0F, 0.0F, 2.0F}};
        auto const                       quad = scene->NewEntity(scene->sceneRoot, "SmokeQuad"_sid);
        ecs::QuadRendererComponent const quadSettings{
            .color       = {0.25F, 0.5F, 0.75F, 1.0F},
            .textureName = "texture.polos_default_white"_sid,
        };

        if (scene->AddComponent(camera, cameraSettings) == nullptr ||
            scene->AddComponent(camera, cameraTransform) == nullptr || !scene->SetActiveCamera(camera) ||
            scene->AddComponent(quad, ecs::TransformComponent{}) == nullptr ||
            scene->AddComponent(quad, quadSettings) == nullptr)
        {
            throw std::runtime_error{"Could not create the integration-test scene"};
        }

        communication::Subscribe<communication::EngineUpdate>([&state = mState](communication::EngineUpdate&) {
            if (++state.updates >= 120U)
            {
                communication::DispatchNow<communication::EngineTerminate>();
            }
        });
        communication::Subscribe<communication::RenderUpdate>([&state = mState](communication::RenderUpdate&) {
            ++state.renderUpdates;
            state.lifecycle.push_back("render");

            gui::SetNextWindowSize({320.0F, 180.0F});
            if (gui::Begin("Integration GUI"))
            {
                gui::Text("A scene and a GUI overlay.");
                if (gui::Image("texture.polos_default_white"_sid, {24.0F, 24.0F}))
                {
                    ++state.guiFrames;
                }
            }
            gui::End();
        });
        communication::Subscribe<communication::EndFrame>([&state = mState](communication::EndFrame&) {
            ++state.completedFrames;
            state.lifecycle.push_back("end-frame");
            if (state.completedFrames == kFrameCount)
            {
                communication::DispatchNow<communication::EngineTerminate>();
            }
        });
    }

    auto Destroy() -> void override
    {
        mState.lifecycle.push_back("destroy");
    }
private:
    SmokeState& mState;
};

TEST(EngineIntegrationTest, RendersSceneAndGuiAcrossFrameSlots)
{
#if defined(__linux__)
    if (std::getenv("DISPLAY") == nullptr && std::getenv("WAYLAND_DISPLAY") == nullptr)
    {
        GTEST_SKIP() << "A window-system display is required for this integration test";
    }
#endif

    SmokeState state;

    auto const result = Engine::Run(new SmokeApplication{state});

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(state.windowCreated);
    EXPECT_TRUE(state.shadersLoaded);
    EXPECT_GE(state.updates, 1U);
    EXPECT_EQ(state.renderUpdates, kFrameCount);
    EXPECT_EQ(state.completedFrames, kFrameCount);
    EXPECT_EQ(state.guiFrames, kFrameCount);
    std::vector<std::string_view> expectedLifecycle{"create"};
    for (unsigned frame = 0U; frame < kFrameCount; ++frame)
    {
        expectedLifecycle.insert(expectedLifecycle.end(), {"render", "end-frame"});
    }
    expectedLifecycle.insert(expectedLifecycle.end(), {"destroy", "destruct"});
    EXPECT_EQ(state.lifecycle, expectedLifecycle);
    EXPECT_EQ(resource::ResourceManager::Instance().GetFile("shader.quad.vertex"_sid), nullptr);
    EXPECT_EQ(resource::ResourceManager::Instance().GetFile("shader.quad.fragment"_sid), nullptr);
}

}// namespace
}// namespace polos::core
