//
// Copyright (c) 2026 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/ecs/systems/editor_camera_system.hpp"

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/view.hpp"
#include "polos/platform/input_state.hpp"

#include <glm/gtc/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>

namespace polos::ecs
{
namespace
{
auto KeyAxis(std::int32_t tPositive, std::int32_t tNegative) -> float
{
    using platform::input::IsKeyDown;
    return static_cast<float>(IsKeyDown(tPositive)) - static_cast<float>(IsKeyDown(tNegative));
}
}// namespace

EditorCameraSystem::~EditorCameraSystem()
{
    reset();
}

auto EditorCameraSystem::Update(Scene* tScene, float const tDeltaTime, GLFWwindow* tWindow) -> void
{
    if (tScene == nullptr)
    {
        reset();
        return;
    }

    View<EntityHandle, Read<CameraComponent, TransformComponent, EditorCameraControllerComponent>> const cameras{
        tScene
    };

    for (auto const& [camera, cameraComponent, currentTransform, controller] : cameras)
    {
        if (camera != tScene->activeCamera)
        {
            continue;
        }

        if (mScene != tScene || mCamera != camera || mWindow != tWindow)
        {
            reset();
            mScene          = tScene;
            mCamera         = camera;
            mWindow         = tWindow;
            mTargetRotation = glm::vec2{currentTransform->rotation};
        }

        bool const looking = platform::input::IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT);
        if (looking != mWasLooking && mWindow != nullptr)
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, looking ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }

        if (looking && mWasLooking)
        {
            auto const mouseDelta = platform::input::gInputState.mouseDelta;
            mTargetRotation.x =
                std::clamp(mTargetRotation.x - (mouseDelta.y * controller->lookSensitivity), -90.0F, 89.9F);
            mTargetRotation.y =
                std::remainder(mTargetRotation.y - (mouseDelta.x * controller->lookSensitivity), 360.0F);
        }

        auto const      deltaTime = std::clamp(tDeltaTime, 0.0F, 0.1F);
        constexpr float smoothness{20.0F};
        auto const      blend    = 1.0F - std::exp(-smoothness * deltaTime);
        glm::vec3       rotation = currentTransform->rotation;
        rotation.x               = glm::mix(rotation.x, mTargetRotation.x, blend);
        rotation.y =
            std::remainder(rotation.y + (std::remainder(mTargetRotation.y - rotation.y, 360.0F) * blend), 360.0F);

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

        auto const targetVelocity    = direction * controller->moveSpeed;
        mVelocity                    = glm::mix(mVelocity, targetVelocity, blend);
        glm::vec3 const nextPosition = currentTransform->position + mVelocity * deltaTime;
        if (currentTransform->position != nextPosition || currentTransform->rotation != rotation)
        {
            auto* transform     = tScene->GetMutableComponent<TransformComponent>(camera);
            transform->position = nextPosition;
            transform->rotation = rotation;
        }
        mWasLooking = looking;
        return;
    }

    reset();
}

auto EditorCameraSystem::reset() -> void
{
    if (mWasLooking && mWindow != nullptr)
    {
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    mScene          = nullptr;
    mCamera         = kInvalidEntityHandle;
    mWindow         = nullptr;
    mTargetRotation = glm::vec2{0.0F};
    mVelocity       = glm::vec3{0.0F};
    mWasLooking     = false;
}

}// namespace polos::ecs
