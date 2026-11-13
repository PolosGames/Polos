#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/components/visibility_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/systems/mesh_instance_sync_system.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

namespace polos::ecs
{
namespace
{
TEST(SceneTest, CreatesAnActiveEditorCameraUnderTheRoot)
{
    auto       scene  = std::make_unique<Scene>();
    auto const camera = scene->activeCamera;

    ASSERT_TRUE(IsEntityValid(camera));
    EXPECT_TRUE(camera != scene->sceneRoot);
    EXPECT_TRUE(scene->GetParent(camera) == scene->sceneRoot);
    EXPECT_EQ(scene->GetComponent<InfoComponent>(camera)->debugName, "Editor Camera"_sid);
    EXPECT_TRUE(scene->HasComponent(camera, ComponentType::kEditorCameraController));

    auto const* settings  = scene->GetComponent<CameraComponent>(camera);
    auto const* transform = scene->GetComponent<TransformComponent>(camera);
    ASSERT_NE(settings, nullptr);
    ASSERT_NE(transform, nullptr);
    EXPECT_EQ(settings->cameraType, CameraType::kPerspective);
    EXPECT_GT(settings->nearZ, 0.0F);
    EXPECT_GT(settings->farZ, settings->nearZ);
    EXPECT_EQ(transform->position, (glm::vec3{0.0F, 0.0F, 5.0F}));
    EXPECT_EQ(transform->rotation, glm::vec3{0.0F});
}

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

TEST(SceneTest, RepeatedWritesShareOneDirtyEntryAndClearingAllowsNewWrites)
{
    auto       scene  = std::make_unique<Scene>();
    auto const entity = scene->activeCamera;
    EXPECT_NE(scene->GetComponent<TransformComponent>(entity), nullptr);
    EXPECT_TRUE(scene->GetDirtyEntities().empty());

    ASSERT_NE(scene->GetMutableComponent<TransformComponent>(entity), nullptr);
    ASSERT_NE(scene->GetMutableComponent<CameraComponent>(entity), nullptr);
    ASSERT_NE(scene->GetMutableComponent<TransformComponent>(entity), nullptr);

    auto dirty = scene->GetDirtyEntities();
    ASSERT_EQ(dirty.size(), 1U);
    EXPECT_TRUE(dirty.front().handle == entity);
    EXPECT_EQ(dirty.front().componentMask.count(), 2U);
    EXPECT_TRUE(dirty.front().componentMask.test(static_cast<std::size_t>(ComponentType::kTransform)));
    EXPECT_TRUE(dirty.front().componentMask.test(static_cast<std::size_t>(ComponentType::kCamera)));

    scene->ClearDirtyEntities();
    EXPECT_TRUE(scene->GetDirtyEntities().empty());

    ASSERT_NE(scene->GetMutableComponent<CameraComponent>(entity), nullptr);
    dirty = scene->GetDirtyEntities();
    ASSERT_EQ(dirty.size(), 1U);
    EXPECT_EQ(dirty.front().componentMask.count(), 1U);
    EXPECT_TRUE(dirty.front().componentMask.test(static_cast<std::size_t>(ComponentType::kCamera)));
}

TEST(SceneTest, RemovingDirtyEntriesKeepsLookupValidForSurvivorsAndRecycledSlots)
{
    auto       scene    = std::make_unique<Scene>();
    auto const retired  = scene->NewEntity();
    auto const survivor = scene->NewEntity();
    for (auto entity : {retired, survivor})
    {
        ASSERT_NE(scene->AddComponent(entity, VisibilityComponent{}), nullptr);
        ASSERT_NE(scene->GetMutableComponent<VisibilityComponent>(entity), nullptr);
    }

    ASSERT_TRUE(scene->RetireEntity(retired));
    ASSERT_NE(scene->GetMutableComponent<VisibilityComponent>(survivor), nullptr);
    ASSERT_EQ(scene->GetDirtyEntities().size(), 1U);
    EXPECT_TRUE(scene->GetDirtyEntities().front().handle == survivor);

    auto const replacement = scene->NewEntity();
    ASSERT_EQ(replacement.index, retired.index);
    ASSERT_NE(replacement.version, retired.version);
    ASSERT_NE(scene->AddComponent(replacement, VisibilityComponent{}), nullptr);
    ASSERT_NE(scene->GetMutableComponent<VisibilityComponent>(replacement), nullptr);
    ASSERT_TRUE(scene->RemoveComponent(survivor, ComponentType::kVisibility));
    ASSERT_EQ(scene->GetDirtyEntities().size(), 1U);
    EXPECT_TRUE(scene->GetDirtyEntities().front().handle == replacement);
}

TEST(SceneTest, MeshInstancesFollowComponentRemovalAndRecycledEntitySlots)
{
    auto                   scene = std::make_unique<Scene>();
    MeshInstanceSyncSystem system;
    auto const             parent = scene->NewEntity();
    auto const             child  = scene->NewEntity(parent);

    ASSERT_NE(scene->AddComponent(child, TransformComponent{.position = {1.0F, 2.0F, 3.0F}}), nullptr);
    system.Update(scene.get());
    auto& renderData      = scene->GetRenderData();
    auto const& instances = renderData.meshInstances;
    auto const first      = instances[child.index];
    EXPECT_EQ(first.parentId, parent.index);
    EXPECT_EQ(first.position, (glm::vec3{1.0F, 2.0F, 3.0F}));

    auto* transform = scene->GetMutableComponent<TransformComponent>(child);
    ASSERT_NE(transform, nullptr);
    transform->position.x = 4.0F;
    system.Update(scene.get());
    scene->ClearDirtyEntities();
    EXPECT_EQ(instances[child.index].position.x, 4.0F);
    auto& pendingUploads = renderData.pendingMeshUploads;
    EXPECT_EQ(pendingUploads.count(child.index), 1U);
    pendingUploads.clear();

    ASSERT_TRUE(scene->RemoveComponent(child, ComponentType::kTransform));
    system.Update(scene.get());
    EXPECT_EQ(instances[child.index].position, glm::vec3{0.0F});
    EXPECT_EQ(instances[child.index].parentId, parent.index);

    ASSERT_TRUE(scene->RetireEntity(parent));
    auto const replacement = scene->NewEntity();
    ASSERT_EQ(replacement.index, child.index);
    ASSERT_NE(replacement.version, child.version);
    ASSERT_NE(scene->AddComponent(replacement, TransformComponent{.position = {5.0F, 0.0F, 0.0F}}), nullptr);
    system.Update(scene.get());
    EXPECT_EQ(instances[parent.index].parentId, rendering::kRootParentId);
    EXPECT_EQ(instances[replacement.index].position.x, 5.0F);
    EXPECT_EQ(instances[replacement.index].parentId, scene->sceneRoot.index);

    ASSERT_TRUE(scene->RetireEntity(replacement));
    system.Update(scene.get());
    EXPECT_EQ(instances[replacement.index].position, glm::vec3{0.0F});
    EXPECT_EQ(pendingUploads.count(replacement.index), 1U);
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
