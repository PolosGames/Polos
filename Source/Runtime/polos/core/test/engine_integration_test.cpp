#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_press.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/mouse_input.hpp"
#include "polos/communication/render_update.hpp"
#include "polos/communication/window_framebuffer_resize.hpp"
#include "polos/core/engine.hpp"
#include "polos/core/i_live_layer.hpp"
#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/mesh_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/scene_manager.hpp"
#include "polos/gui/backend.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/cache/mesh_cache.hpp"
#include "polos/rendering/common.hpp"

#include <gtest/gtest.h>
#include <imgui.h>

#include <GLFW/glfw3.h>

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
    bool                          cameraMoved{false};
    bool                          cursorCaptured{false};
    bool                          cursorReleased{false};
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
        auto&      platform    = platform::PlatformManager::Get();
        auto const framebuffer = platform.GetWindowSurface().GetFramebufferSize();
        mState.windowCreated   = platform.GetMainWindow() != nullptr && framebuffer.width > 0 && framebuffer.height > 0;

        auto*      scene           = ecs::SceneManager::Get().CreateScene();
        auto const camera          = scene->activeCamera;
        auto*      cameraTransform = scene->GetMutableComponent<ecs::TransformComponent>(camera);
        auto const cube            = scene->NewEntity(scene->sceneRoot, "SmokeCube"_sid);
        auto const meshIndex       = rendering::MeshCache::Get().GetMeshIndexByHandle(rendering::kDefaultCube);

        if (cameraTransform == nullptr || !scene->HasComponent(camera, ecs::CameraComponent::kType) ||
            !scene->HasComponent(camera, ecs::EditorCameraControllerComponent::kType) ||
            meshIndex == rendering::kInvalidMeshId || scene->AddComponent(cube, ecs::TransformComponent{}) == nullptr ||
            scene->AddComponent(cube, ecs::MeshRendererComponent{rendering::kDefaultCube}) == nullptr)
        {
            throw std::runtime_error{"Could not create the integration-test scene"};
        }
        cameraTransform->position = {0.0F, 0.0F, 2.0F};

        communication::Subscribe<communication::EngineUpdate>(
            [&state = mState, scene, cube](communication::EngineUpdate&) {
                ++state.updates;
                if (state.updates == 1U)
                {
                    communication::DispatchNow<communication::MouseInput>(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
                    communication::DispatchNow<communication::KeyPress>(GLFW_KEY_W);
                }
                if (state.completedFrames == 1U && scene->HasComponent(cube, ecs::ComponentType::kMeshRenderer))
                {
                    EXPECT_TRUE(scene->RemoveComponent(cube, ecs::ComponentType::kMeshRenderer));
                }
                if (state.completedFrames == 2U && !scene->HasComponent(cube, ecs::ComponentType::kMeshRenderer))
                {
                    EXPECT_NE(scene->AddComponent(cube, ecs::MeshRendererComponent{rendering::kDefaultCube}), nullptr);
                }
                if (state.updates == 3U)
                {
                    communication::DispatchNow<communication::MouseInput>(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE);
                    communication::DispatchNow<communication::KeyRelease>(GLFW_KEY_W);
                }
                if (state.updates >= 120U)
                {
                    communication::DispatchNow<communication::EngineTerminate>();
                }
            });
        communication::Subscribe<communication::RenderUpdate>([&state = mState, scene, cube, meshIndex,
                                                                  cameraTransform](communication::RenderUpdate&) {
            auto const expectedMesh = state.completedFrames == 1U ? rendering::kInvalidMeshId : meshIndex;
            EXPECT_EQ(scene->GetRenderData().meshInstances[cube.index].meshId, expectedMesh)
                << "Frame " << state.completedFrames;
            ++state.renderUpdates;
            state.lifecycle.push_back("render");
            state.cameraMoved     = state.cameraMoved || cameraTransform->position.z < 2.0F;
            auto const cursorMode = glfwGetInputMode(platform::PlatformManager::Get().GetMainWindow(), GLFW_CURSOR);
            state.cursorCaptured  = state.cursorCaptured || cursorMode == GLFW_CURSOR_DISABLED;
            state.cursorReleased  = state.cursorReleased || (state.cursorCaptured && cursorMode == GLFW_CURSOR_NORMAL);

            ImGui::SetNextWindowSize({320.0F, 180.0F});
            if (ImGui::Begin("Integration GUI"))
            {
                ImGui::TextUnformatted("A scene and a GUI overlay.");
                auto const texture = gui::backend::GetImageTexture(rendering::kDefaultTexture);
                EXPECT_NE(texture, 0U);
                ImGui::Image(texture, {24.0F, 24.0F});
                float values[]{0.0F, 0.0F, 0.0F};
                ImGui::DragFloat2("Two values", values);
                ImGui::DragFloat3("Three values", values);
                ++state.guiFrames;
            }
            ImGui::End();
        });
        communication::Subscribe<communication::EndFrame>([&state = mState](communication::EndFrame&) {
            ++state.completedFrames;
            state.lifecycle.push_back("end-frame");
            if (state.completedFrames == 1U)
            {
                // Exercise swapchain recreation before checking the remaining frames.
                int width{};
                int height{};
                glfwGetFramebufferSize(platform::PlatformManager::Get().GetMainWindow(), &width, &height);
                communication::DispatchNow<communication::WindowFramebufferResize>(width, height);
            }
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
    EXPECT_TRUE(state.cameraMoved);
    EXPECT_TRUE(state.cursorCaptured);
    EXPECT_TRUE(state.cursorReleased);
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
}

}// namespace
}// namespace polos::core
