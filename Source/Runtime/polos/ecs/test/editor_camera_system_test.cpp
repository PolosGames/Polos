#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/systems/editor_camera_system.hpp"
#include "polos/input/input_state.hpp"
#include "polos/scene/scene.hpp"

#include <gtest/gtest.h>

#include <GLFW/glfw3.h>

#include <array>
#include <cmath>
#include <memory>

namespace polos::ecs
{
namespace
{
class EditorCameraSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        input::gInputState   = {};
        scene                = std::make_unique<scene::Scene>();
        transform            = scene->GetMutableComponent<TransformComponent>(scene->activeCamera);
    }

    void TearDown() override
    {
        input::gInputState = {};
    }

    static auto Hold(int tKey) -> void
    {
        input::gInputState.keysDown.set(static_cast<std::size_t>(tKey));
    }

    std::unique_ptr<scene::Scene> scene;
    TransformComponent*    transform{};
    EditorCameraSystem     system;
};

TEST_F(EditorCameraSystemTest, MovementRequiresRightMouseAndSmoothlyStopsOnRelease)
{
    auto const start = transform->position;
    Hold(GLFW_KEY_W);
    system.Update(scene.get(), 0.05F);
    EXPECT_EQ(transform->position, start);

    Hold(GLFW_MOUSE_BUTTON_RIGHT);
    system.Update(scene.get(), 0.05F);
    auto const movingPosition = transform->position;
    auto const firstDistance  = start.z - movingPosition.z;
    EXPECT_GT(firstDistance, 0.0F);
    EXPECT_LT(firstDistance, 5.0F * 0.05F);

    input::gInputState.keysDown.reset(GLFW_MOUSE_BUTTON_RIGHT);
    system.Update(scene.get(), 0.05F);
    auto const stoppingDistance = movingPosition.z - transform->position.z;
    EXPECT_GT(stoppingDistance, 0.0F);
    EXPECT_LT(stoppingDistance, firstDistance);
    for (int i{0}; i < 30; ++i)
    {
        system.Update(scene.get(), 0.05F);
    }
    auto const stoppedPosition = transform->position;
    system.Update(scene.get(), 0.05F);
    EXPECT_EQ(transform->position, stoppedPosition);
}

TEST_F(EditorCameraSystemTest, MovementUsesCameraHeadingWorldVerticalAndNormalizedDiagonals)
{
    struct Movement
    {
        std::array<int, 3> keys;
        float              yaw;
        glm::vec3          direction;
    };
    std::array const movements{
        Movement{{GLFW_KEY_W, -1, -1}, 0.0F, {0.0F, 0.0F, -1.0F}},
        Movement{{GLFW_KEY_S, -1, -1}, 0.0F, {0.0F, 0.0F, 1.0F}},
        Movement{{GLFW_KEY_A, -1, -1}, 0.0F, {-1.0F, 0.0F, 0.0F}},
        Movement{{GLFW_KEY_D, -1, -1}, 0.0F, {1.0F, 0.0F, 0.0F}},
        Movement{{GLFW_KEY_SPACE, -1, -1}, 90.0F, {0.0F, 1.0F, 0.0F}},
        Movement{{GLFW_KEY_LEFT_CONTROL, -1, -1}, 90.0F, {0.0F, -1.0F, 0.0F}},
        Movement{{GLFW_KEY_W, -1, -1}, 90.0F, {-1.0F, 0.0F, 0.0F}},
        Movement{{GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_SPACE}, 0.0F, glm::normalize(glm::vec3{1.0F, 1.0F, -1.0F})},
    };
    float distance{0.0F};
    for (auto const& movement : movements)
    {
        system.Update(nullptr, 0.0F);
        input::gInputState   = {};
        transform->position  = glm::vec3{0.0F};
        transform->rotation  = {0.0F, movement.yaw, 0.0F};
        Hold(GLFW_MOUSE_BUTTON_RIGHT);
        for (auto key : movement.keys)
        {
            if (key >= 0)
            {
                Hold(key);
            }
        }
        system.Update(scene.get(), 0.1F);
        auto const actualDistance = glm::length(transform->position);
        ASSERT_GT(actualDistance, 0.0F);
        EXPECT_LT(glm::length(transform->position / actualDistance - movement.direction), 0.0001F);
        if (distance == 0.0F)
        {
            distance = actualDistance;
        }
        EXPECT_NEAR(actualDistance, distance, 0.0001F);
    }
}

TEST_F(EditorCameraSystemTest, MouseLookIgnoresCaptureDeltaClampsPitchAndWrapsYawSmoothly)
{
    transform->rotation             = {0.0F, 179.0F, 0.0F};
    input::gInputState.mouseDelta   = {1000.0F, 1000.0F};
    Hold(GLFW_MOUSE_BUTTON_RIGHT);
    system.Update(scene.get(), 0.1F);
    EXPECT_EQ(transform->rotation, (glm::vec3{0.0F, 179.0F, 0.0F}));

    input::gInputState.mouseDelta = {-40.0F, -10000.0F};
    system.Update(scene.get(), 0.1F);
    auto const yawChange = std::remainder(transform->rotation.y - 179.0F, 360.0F);
    EXPECT_GT(yawChange, 0.0F);
    EXPECT_LT(yawChange, 4.0F);
    EXPECT_GT(transform->rotation.x, 0.0F);
    EXPECT_LT(transform->rotation.x, 89.9F);

    input::gInputState.mouseDelta = glm::vec2{0.0F};
    for (int i{0}; i < 20; ++i)
    {
        system.Update(scene.get(), 0.1F);
    }
    EXPECT_NEAR(transform->rotation.x, 89.9F, 0.001F);
    EXPECT_NEAR(transform->rotation.y, -177.0F, 0.001F);
}

TEST_F(EditorCameraSystemTest, CameraAndSceneChangesDiscardMomentumAndUseTheNewRotation)
{
    Hold(GLFW_MOUSE_BUTTON_RIGHT);
    Hold(GLFW_KEY_W);
    system.Update(scene.get(), 0.1F);

    auto const otherCamera = scene->NewEntity();
    scene->AddComponent(otherCamera, CameraComponent{});
    scene->AddComponent(otherCamera, EditorCameraControllerComponent{});
    auto* otherTransform = scene->AddComponent(
        otherCamera,
        TransformComponent{.position = {1.0F, 2.0F, 3.0F}, .rotation = {20.0F, 30.0F, 0.0F}});
    ASSERT_TRUE(scene->SetActiveCamera(otherCamera));
    input::gInputState = {};
    system.Update(scene.get(), 0.1F);
    EXPECT_EQ(otherTransform->position, (glm::vec3{1.0F, 2.0F, 3.0F}));
    EXPECT_EQ(otherTransform->rotation, (glm::vec3{20.0F, 30.0F, 0.0F}));

    auto otherScene = std::make_unique<scene::Scene>();
    system.Update(otherScene.get(), 0.1F);
    EXPECT_EQ(
        otherScene->GetComponent<TransformComponent>(otherScene->activeCamera)->position,
        (glm::vec3{0.0F, 0.0F, 5.0F}));
}

TEST_F(EditorCameraSystemTest, SkipsNonEditorCamerasAndMissingOrRetiredCameras)
{
    Hold(GLFW_MOUSE_BUTTON_RIGHT);
    Hold(GLFW_KEY_W);
    auto const camera = scene->activeCamera;
    auto const start  = transform->position;
    ASSERT_TRUE(scene->RemoveComponent(camera, ComponentType::kEditorCameraController));
    system.Update(scene.get(), 0.1F);
    EXPECT_EQ(transform->position, start);

    scene->AddComponent(camera, EditorCameraControllerComponent{});
    ASSERT_TRUE(scene->RemoveComponent(camera, ComponentType::kTransform));
    system.Update(scene.get(), 0.1F);
    ASSERT_TRUE(scene->RetireEntity(camera));
    system.Update(scene.get(), 0.1F);
    system.Update(nullptr, 0.1F);
    EXPECT_TRUE(scene->activeCamera == kInvalidEntityHandle);
}

}// namespace
}// namespace polos::ecs
