//
// Copyright (c) 2026 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include <glm/gtc/quaternion.hpp>
#include <polos/communication/engine_terminate.hpp>
#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/key_press.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/base_live_layer.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/ecs/components/camera_component.hpp>
#include <polos/ecs/components/editor_camera_controller_component.hpp>
#include <polos/ecs/components/quad_renderer_component.hpp>
#include <polos/ecs/components/transform_component.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/ecs/scene_manager.hpp>
#include <polos/gui/gui.hpp>
#include <polos/platform/input_state.hpp>
#include <polos/platform/platform_manager.hpp>
#include <polos/resource/image.hpp>
#include <polos/resource/resource_manager.hpp>
#include <polos/utils/assert.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <format>
#include <numbers>
#include <random>

namespace tux_quads
{
namespace
{
constexpr std::int32_t kCameraFov{55};

constexpr std::size_t kQuadCount{500'000U};
constexpr float       kWaveHalfWidth{12.0F};
constexpr float       kWaveAmplitude{2.5F};
constexpr float       kWaveLength{12.0F};

auto KeyAxis(std::int32_t tPositive, std::int32_t tNegative) -> float
{
    using polos::platform::input::IsKeyDown;
    return static_cast<float>(IsKeyDown(tPositive)) - static_cast<float>(IsKeyDown(tNegative));
}
}// namespace

class TuxQuadsApp final : public polos::core::BaseLiveLayer
{
public:
    auto Create() -> void override
    {
        polos::platform::PlatformManager::Instance().ChangeWindowTitle("Polos - 500,000 Tux Quads");
        auto* scene = polos::ecs::SceneManager::CreateScene();

        auto const                           camera = scene->NewEntity(scene->sceneRoot, "Editor Camera"_sid);
        polos::ecs::TransformComponent const cameraTransform{
            .position = {0.0F, 30.0F, 0.0F},
            .rotation = {-90.0F, 0.0F, 0.0F},
        };
        polos::ecs::CameraComponent const cameraSettings{
            .nearZ = 0.1F,
            .farZ  = 1000.0F,
            .fov   = kCameraFov,
        };
        mCameraTransform = scene->AddComponent(camera, cameraTransform);
        scene->AddComponent(camera, cameraSettings);
        mCameraController = scene->AddComponent(camera, polos::ecs::EditorCameraControllerComponent{});
        mTargetRotation   = glm::vec2{cameraTransform.rotation};
        PL_ASSERT(scene->SetActiveCamera(camera), "Could not activate the overview camera.");

        auto const tux = polos::resource::ResourceManager::Instance().GetImage("texture.tux"_sid);
        PL_ASSERT(tux != nullptr, "The Tux texture must be in the resource catalog.");
        constexpr float quadWidth{0.3F};
        auto const      quadHeight = quadWidth * static_cast<float>(tux->height) / static_cast<float>(tux->width);
        polos::ecs::QuadRendererComponent const renderer{.textureName = "texture.tux"_sid};

        std::mt19937                          random{42U};
        std::uniform_real_distribution<float> sampleX{-kWaveHalfWidth, kWaveHalfWidth};
        constexpr float                       waveFrequency{2.0F * std::numbers::pi_v<float> / kWaveLength};
        for (std::size_t i{0U}; i < kQuadCount; ++i)
        {
            auto const                           x    = sampleX(random);
            auto const                           z    = kWaveAmplitude * std::sin(x * waveFrequency);
            auto const                           quad = scene->NewEntity(scene->sceneRoot, "Tux Quad"_sid);
            polos::ecs::TransformComponent const transform{
                .position = {x, 0.0F, z},
                .rotation = {-90.0F, 0.0F, 0.0F},
                .scale    = {quadWidth, quadHeight, 1.0F},
            };
            PL_ASSERT(scene->AddComponent(quad, transform) != nullptr, "Could not add the quad transform.");
            PL_ASSERT(scene->AddComponent(quad, renderer) != nullptr, "Could not add the quad renderer.");
        }

        auto const size = polos::platform::PlatformManager::Instance().GetWindowSurface().GetFramebufferSize();
        if (size.width > 0U && size.height > 0U)
        {
            auto const      aspect         = static_cast<float>(size.width) / static_cast<float>(size.height);
            auto const      halfFovTangent = std::tan(glm::radians(static_cast<float>(kCameraFov)) * 0.5F);
            auto const      halfWidth      = kWaveHalfWidth + quadWidth * 0.5F;
            auto const      halfDepth      = kWaveAmplitude + quadHeight * 0.5F;
            constexpr float margin{1.15F};
            mCameraTransform->position.y = margin * std::max(halfWidth / aspect, halfDepth) / halfFovTangent;
        }

        polos::communication::Subscribe<polos::communication::EngineUpdate>(
            [this](polos::communication::EngineUpdate& tEvent) {
                updateCamera(tEvent.deltaTime);
            });

        polos::communication::Subscribe<polos::communication::RenderUpdate>(
            [this](polos::communication::RenderUpdate& tEvent) {
                drawFpsOverlay(tEvent.deltaTime);
            });

        polos::communication::Subscribe<polos::communication::KeyPress>([](polos::communication::KeyPress& tEvent) {
            if (tEvent.key == GLFW_KEY_ESCAPE)
            {
                polos::communication::DispatchNow<polos::communication::EngineTerminate>();
            }
        });
    }

    auto Name() const -> char const* override
    {
        return "TuxQuads";
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
        gui::SetNextWindowPosition({8.0F, 8.0F});
        gui::SetNextWindowBackgroundAlpha(0.65F);
        if (gui::Begin("TuxFpsOverlay", nullptr, flags))
        {
            gui::Text(mFps > 0.0F ? std::format("FPS: {:.1f}", mFps) : "FPS: --");
        }
        gui::End();
    }

    auto updateCamera(float tDeltaTime) -> void
    {
        using polos::platform::input::IsKeyDown;

        bool const looking = IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT);
        if (looking != mWasLooking)
        {
            auto* window = polos::platform::PlatformManager::Instance().GetMainWindow();
            glfwSetInputMode(window, GLFW_CURSOR, looking ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }

        if (looking)
        {
            if (mWasLooking)
            {
                auto const mouseDelta = polos::platform::input::g_input_state.mouseDelta;
                mTargetRotation.x =
                    std::clamp(mTargetRotation.x - mouseDelta.y * mCameraController->lookSensitivity, -90.0F, 89.9F);
                mTargetRotation.y =
                    std::remainder(mTargetRotation.y - mouseDelta.x * mCameraController->lookSensitivity, 360.0F);
            }
        }

        auto const      deltaTime = std::clamp(tDeltaTime, 0.0F, 0.1F);
        constexpr float smoothness{20.0F};
        auto const      blend    = 1.0F - std::exp(-smoothness * deltaTime);
        auto&           rotation = mCameraTransform->rotation;
        rotation.x               = glm::mix(rotation.x, mTargetRotation.x, blend);
        rotation.y =
            std::remainder(rotation.y + std::remainder(mTargetRotation.y - rotation.y, 360.0F) * blend, 360.0F);

        glm::vec3 direction{0.0F};
        if (looking)
        {
            auto const yaw     = glm::angleAxis(glm::radians(rotation.y), glm::vec3{0.0F, 1.0F, 0.0F});
            auto const pitch   = glm::angleAxis(glm::radians(rotation.x), glm::vec3{1.0F, 0.0F, 0.0F});
            auto const forward = yaw * pitch * glm::vec3{0.0F, 0.0F, -1.0F};
            auto const right   = yaw * glm::vec3{1.0F, 0.0F, 0.0F};

            direction = forward * KeyAxis(GLFW_KEY_W, GLFW_KEY_S) + right * KeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
                        glm::vec3{0.0F, 1.0F, 0.0F} * KeyAxis(GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL);
            if (glm::length(direction) > 0.0F)
            {
                direction = glm::normalize(direction);
            }
        }

        auto const targetVelocity = direction * mCameraController->moveSpeed;
        mCameraVelocity           = glm::mix(mCameraVelocity, targetVelocity, blend);
        mCameraTransform->position += mCameraVelocity * deltaTime;
        mWasLooking = looking;
    }

    polos::ecs::TransformComponent*              mCameraTransform{nullptr};
    polos::ecs::EditorCameraControllerComponent* mCameraController{nullptr};
    glm::vec2                                    mTargetRotation{0.0F};
    glm::vec3                                    mCameraVelocity{0.0F};
    bool                                         mWasLooking{false};

    float         mFps{0.0F};
    float         mFpsSampleTime{0.0F};
    std::uint32_t mFpsSampleFrames{0U};
};

}// namespace tux_quads

namespace polos
{
auto CreateApplication(int /*argc*/, char** /*argv*/) -> core::ILiveLayer*
{
    return new tux_quads::TuxQuadsApp{};
}
}// namespace polos
