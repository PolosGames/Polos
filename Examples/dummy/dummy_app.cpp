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
#include <glm/gtc/quaternion.hpp>
#include <polos/communication/engine_terminate.hpp>
#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/ecs/components/camera_component.hpp>
#include <polos/ecs/components/editor_camera_controller_component.hpp>
#include <polos/ecs/components/transform_component.hpp>
#include <polos/ecs/entity.hpp>
#include <polos/ecs/scene_manager.hpp>
#include <polos/logging/log_macros.hpp>
#include <polos/platform/i_window_surface.hpp>
#include <polos/platform/input_state.hpp>
#include <polos/platform/platform_manager.hpp>
#include <polos/rendering/rendering_api.hpp>
#include <polos/resource/image.hpp>
#include <polos/resource/model.hpp>
#include <polos/resource/resource_manager.hpp>

#include <GLFW/glfw3.h>

#include <filesystem>

namespace dummy_app
{

namespace
{
constexpr std::int32_t kCamFov = 60;
constexpr float        kNearZ{0.1F};
constexpr float        kFarZ{10.0F};

polos::ecs::EntityHandle                     editorCamera;
polos::ecs::TransformComponent*              editorCameraTransform{nullptr};
polos::ecs::CameraComponent*                 editorCameraComponent{nullptr};
polos::ecs::EditorCameraControllerComponent* editorCameraController{nullptr};

static constexpr std::float_t kSensitivity = 3.0F;
static constexpr std::float_t kCameraSpeed{1.0F};
}// namespace

DummyApp::DummyApp()  = default;
DummyApp::~DummyApp() = default;

void DummyApp::Create()
{
    using namespace polos::communication;

    Subscribe<EngineUpdate>([this](EngineUpdate& t_event) {
        onEngineUpdate(t_event);
    });

    Subscribe<RenderUpdate>([this](RenderUpdate& t_event) {
        onRenderUpdate(t_event);
    });

    mScene = polos::ecs::SceneManager::CreateScene();

    editorCamera = mScene->NewEntity();

    editorCameraTransform = mScene->AddComponent(
        editorCamera,
        polos::ecs::TransformComponent{
            .position = glm::vec3(0.0F, 0.0F, 2.0F),
            .rotation = glm::vec3(0.0F, 0.0F, 0.0F),
        });

    editorCameraComponent = mScene->AddComponent(
        editorCamera,
        polos::ecs::CameraComponent{
            .cameraType = polos::ecs::CameraType::kPerspective,
            .nearZ      = kNearZ,
            .farZ       = kFarZ,
            .fov        = kCamFov,
        });

    editorCameraController = mScene->AddComponent(
        editorCamera,
        polos::ecs::EditorCameraControllerComponent{
            .moveSpeed       = kCameraSpeed,
            .lookSensitivity = kSensitivity,
        });

    mScene->SetActiveCamera(editorCamera);
}

void DummyApp::Destroy()
{}

char const* DummyApp::Name() const
{
    return "DummyApp";
}

auto DummyApp::getWindowSize() -> polos::platform::FramebufferSize
{
    return polos::platform::PlatformManager::Instance().GetWindowSurface().GetFramebufferSize();
}

auto DummyApp::onEngineUpdate(polos::communication::EngineUpdate& t_event) -> void
{
    moveCamera(editorCameraTransform, t_event.deltaTime);
}

auto DummyApp::onRenderUpdate(polos::communication::RenderUpdate& /**/) -> void
{
    //LogInfo("Render Thread Update");
}

namespace
{

// pitch never needs wraparound, always clamped well inside +-180
auto ComputeSmoothedPitch(std::float_t t_current_pitch, std::float_t t_mouse_delta_y, std::float_t t_interpolation)
    -> std::float_t
{
    static constexpr std::float_t kMaxPitch = 89.99F;

    std::float_t target_pitch = t_current_pitch;
    target_pitch -= t_mouse_delta_y * kSensitivity;
    target_pitch = std::clamp(target_pitch, -kMaxPitch, kMaxPitch);

    return glm::mix(t_current_pitch, target_pitch, t_interpolation);
}

auto ComputeSmoothedYaw(std::float_t t_current_yaw, std::float_t t_mouse_delta_x, std::float_t t_interpolation)
    -> std::float_t
{
    static constexpr std::float_t kMaxDeg     = 360.0F;
    static constexpr std::float_t kHalfMaxDeg = kMaxDeg / 2.0F;

    std::float_t target_yaw = t_current_yaw;
    target_yaw -= t_mouse_delta_x * kSensitivity;
    target_yaw = std::fmod(target_yaw, kMaxDeg);
    if (0.0F > target_yaw)
    {
        target_yaw += kMaxDeg;
    }

    // we need this because shortest path between 5 -> 355 should be 10 and not 350
    // which without this, would be the case.
    std::float_t delta_yaw = std::fmod(target_yaw - t_current_yaw, kMaxDeg);
    if (kHalfMaxDeg < delta_yaw)
    {
        delta_yaw -= kMaxDeg;
    }
    else if (-kHalfMaxDeg > delta_yaw)
    {
        delta_yaw += kMaxDeg;
    }

    std::float_t blended_yaw = std::fmod(glm::mix(t_current_yaw, t_current_yaw + delta_yaw, t_interpolation), kMaxDeg);
    if (0.0F > blended_yaw)
    {
        blended_yaw += kMaxDeg;
    }
    return blended_yaw;
}

}// namespace

auto DummyApp::moveCamera(polos::ecs::TransformComponent* tTransform, float tDelta) -> void
{
    static constexpr std::float_t kSmoothness = 20.0F;

    if (polos::platform::input::IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT))
    {
        // clamp so a slow frame can't push the blend past the target
        std::float_t const interpolation = std::min(tDelta * kSmoothness, 1.0F);

        std::float_t const screen_h = static_cast<std::float_t>(getWindowSize().height);
        std::float_t const screen_w = static_cast<std::float_t>(getWindowSize().width);

        std::float_t const mouse_delta_y =
            std::clamp(polos::platform::input::g_input_state.mouseDelta.y, -screen_h, screen_h);
        std::float_t const mouse_delta_x =
            std::clamp(polos::platform::input::g_input_state.mouseDelta.x, -screen_w, screen_w);

        tTransform->rotation.x = ComputeSmoothedPitch(tTransform->rotation.x, mouse_delta_y, interpolation);
        tTransform->rotation.y = ComputeSmoothedYaw(tTransform->rotation.y, mouse_delta_x, interpolation);

        // Compute forward vector from pitch and yaw
        glm::quat const pitch_quat = glm::angleAxis(glm::radians(tTransform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat const yaw_quat   = glm::angleAxis(glm::radians(tTransform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

        // Apply Yaw first, then Pitch
        glm::quat const orientation = glm::normalize(yaw_quat * pitch_quat);

        glm::vec3 const forward = orientation * glm::vec3(0.0F, 0.0F, -1.0F);

        glm::vec3 const right = glm::normalize(orientation * glm::vec3(1.0F, 0.0F, 0.0F) * glm::vec3(1.0F, 0.0F, 1.0F));

        if (polos::platform::input::IsKeyDown(GLFW_KEY_W))
        {
            tTransform->position += forward * tDelta * kCameraSpeed;
        }
        if (polos::platform::input::IsKeyDown(GLFW_KEY_S))
        {
            tTransform->position -= forward * tDelta * kCameraSpeed;
        }
        if (polos::platform::input::IsKeyDown(GLFW_KEY_A))
        {
            tTransform->position -= right * tDelta * kCameraSpeed;
        }
        if (polos::platform::input::IsKeyDown(GLFW_KEY_D))
        {
            tTransform->position += right * tDelta * kCameraSpeed;
        }
        if (polos::platform::input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
        {
            tTransform->position.y -= tDelta * kCameraSpeed;
        }
        if (polos::platform::input::IsKeyDown(GLFW_KEY_SPACE))
        {
            tTransform->position.y += tDelta * kCameraSpeed;
        }
    }
}

}// namespace dummy_app

namespace polos
{

auto CreateApplication(int /*argc*/, char** /*argv*/) -> ::polos::core::ILiveLayer*
{
    return new dummy_app::DummyApp{};//NOLINT(cppcoreguidelines-owning-memory)
}

}// namespace polos
