#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/visibility_component.hpp"
#include "polos/ecs/scene.hpp"

#include <gtest/gtest.h>

#include <memory>

namespace polos::ecs
{
namespace
{
TEST(SceneTest, ComponentRemovalAllowsReaddingWithoutAffectingOtherEntities)
{
    auto                      scene  = std::make_unique<Scene>();
    auto const                first  = scene->NewEntity();
    auto const                second = scene->NewEntity();
    VisibilityComponent const hidden{.isVisible = false};
    VisibilityComponent const visible{.isVisible = true};
    auto*                     original = scene->AddComponent(first, hidden);
    ASSERT_NE(original, nullptr);
    ASSERT_NE(scene->AddComponent(second, visible), nullptr);

    EXPECT_EQ(scene->AddComponent(first, visible), nullptr);
    EXPECT_FALSE(original->isVisible);

    ASSERT_TRUE(scene->RemoveComponent(first, ComponentType::kVisibility));

    EXPECT_FALSE(scene->HasComponent(first, ComponentType::kVisibility));
    EXPECT_EQ(scene->GetComponent<VisibilityComponent>(first), nullptr);
    EXPECT_FALSE(scene->RemoveComponent(first, ComponentType::kVisibility));
    EXPECT_TRUE(scene->GetComponent<VisibilityComponent>(second)->isVisible);

    auto const* replacement = scene->AddComponent(first, visible);

    ASSERT_NE(replacement, nullptr);
    EXPECT_TRUE(replacement->isVisible);
    EXPECT_TRUE(scene->HasComponent(first, ComponentType::kVisibility));
}

TEST(SceneTest, RecycledSlotsRejectOldHandlesWithoutChangingTheNewEntity)
{
    auto       scene   = std::make_unique<Scene>();
    auto const retired = scene->NewEntity();
    ASSERT_NE(scene->AddComponent(retired, VisibilityComponent{.isVisible = false}), nullptr);
    ASSERT_TRUE(scene->RetireEntity(retired));
    auto const replacement = scene->NewEntity();
    ASSERT_EQ(replacement.index, retired.index);
    ASSERT_NE(replacement.version, retired.version);

    EXPECT_FALSE(scene->HasComponent(replacement, ComponentType::kVisibility));
    EXPECT_EQ(scene->GetComponent<VisibilityComponent>(retired), nullptr);
    EXPECT_EQ(scene->AddComponent(retired, VisibilityComponent{}), nullptr);
    EXPECT_FALSE(scene->RetireEntity(retired));
    EXPECT_FALSE(scene->RemoveComponent(retired, ComponentType::kInfo));

    auto const* component = scene->AddComponent(replacement, VisibilityComponent{});

    ASSERT_NE(component, nullptr);
    EXPECT_TRUE(component->isVisible);
    EXPECT_EQ(scene->GetEntityAtIndex(replacement.index).version, replacement.version);
}

TEST(SceneTest, ActiveCameraSelectionRejectsEntitiesWithoutCameraComponents)
{
    auto       scene  = std::make_unique<Scene>();
    auto const camera = scene->NewEntity();
    auto const other  = scene->NewEntity();
    ASSERT_NE(scene->AddComponent(camera, CameraComponent{}), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(camera));

    EXPECT_FALSE(scene->SetActiveCamera(other));
    EXPECT_TRUE(scene->activeCamera == camera);
    EXPECT_FALSE(scene->SetActiveCamera(kInvalidEntityHandle));
    EXPECT_TRUE(scene->activeCamera == camera);
}

}// namespace
}// namespace polos::ecs
