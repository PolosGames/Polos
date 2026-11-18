//
// Copyright (c) 2026 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_ECS_SYSTEMS_EDITOR_CAMERA_SYSTEM_HPP
#define POLOS_ECS_SYSTEMS_EDITOR_CAMERA_SYSTEM_HPP

#include "polos/ecs/entity.hpp"
#include "polos/polos_api.hpp"

#include <glm/glm.hpp>

struct GLFWwindow;

namespace polos::ecs
{

class Scene;

/// Controls the active camera when it has an EditorCameraControllerComponent.
class POLOS_API EditorCameraSystem
{
public:
    EditorCameraSystem() = default;
    ~EditorCameraSystem();

    EditorCameraSystem(EditorCameraSystem const&)                    = delete;
    EditorCameraSystem(EditorCameraSystem&&)                         = default;
    auto operator=(EditorCameraSystem const&) -> EditorCameraSystem& = delete;
    auto operator=(EditorCameraSystem&&) -> EditorCameraSystem&      = default;

    /// A null window allows camera updates without changing the cursor mode.
    auto Update(Scene* tScene, float tDeltaTime, GLFWwindow* tWindow = nullptr) -> void;
private:
    auto reset() -> void;

    Scene*       mScene{nullptr};
    EntityHandle mCamera{kInvalidEntityHandle};
    GLFWwindow*  mWindow{nullptr};
    glm::vec2    mTargetRotation{0.0F};
    glm::vec3    mVelocity{0.0F};
    bool         mWasLooking{false};
};

}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEMS_EDITOR_CAMERA_SYSTEM_HPP
