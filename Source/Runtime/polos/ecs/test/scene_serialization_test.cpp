#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/light_component.hpp"
#include "polos/ecs/components/mesh_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/components/visibility_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/scene_manager.hpp"
#include "polos/ecs/view.hpp"
#include "polos/utils/string_map.hpp"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace polos::ecs
{
class SceneSerializationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        manager                 = std::make_unique<SceneManager>();
        SceneManager::sInstance = manager.get();

        scene = manager->CreateScene("SerializationTest"_sid);
        // These fixtures build their own camera and hierarchy, including root-only scenes.
        ASSERT_TRUE(scene->RetireEntity(scene->activeCamera));

        directory = std::filesystem::temp_directory_path() /
                    ("polos-save-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directory(directory);
    }

    void TearDown() override
    {
        SceneManager::sInstance = nullptr;
        std::filesystem::remove_all(directory);
    }

    auto Save(std::filesystem::path const& path) -> bool
    {
        return manager->SaveCurrentScene(utils::RegisterString(path.string()));
    }

    std::unique_ptr<SceneManager> manager;
    Scene*                        scene{};
    std::filesystem::path         directory;
};

namespace
{
auto ExpectChildren(Scene& tScene, EntityHandle tParent, std::initializer_list<utils::string_id> tNames) -> void
{
    auto const* parentInfo = tScene.GetComponent<InfoComponent>(tParent);

    ASSERT_NE(parentInfo, nullptr);
    EXPECT_EQ(parentInfo->childCount, tNames.size());

    auto child = tScene.GetChild(tParent);
    for (auto const name : tNames)
    {
        ASSERT_TRUE(child != kInvalidEntityHandle) << "Missing child";
        auto const* childInfo = tScene.GetComponent<InfoComponent>(child);
        ASSERT_NE(childInfo, nullptr);

        EXPECT_EQ(childInfo->debugName, name);
        EXPECT_TRUE(tScene.GetParent(child) == tParent);
        child = tScene.GetNextSibling(child);
    }

    EXPECT_TRUE(child == kInvalidEntityHandle);
}

auto ExpectComponentEqual(VisibilityComponent const& tExpected, VisibilityComponent const& tActual) -> void
{
    EXPECT_EQ(tActual.isVisible, tExpected.isVisible);
}

auto ExpectComponentEqual(CameraComponent const& tExpected, CameraComponent const& tActual) -> void
{
    EXPECT_EQ(tActual.cameraType, tExpected.cameraType);
    EXPECT_FLOAT_EQ(tActual.nearZ, tExpected.nearZ);
    EXPECT_FLOAT_EQ(tActual.farZ, tExpected.farZ);
    EXPECT_FLOAT_EQ(tActual.fov, tExpected.fov);
    EXPECT_FLOAT_EQ(tActual.size, tExpected.size);
}

auto ExpectComponentEqual(TransformComponent const& tExpected, TransformComponent const& tActual) -> void
{
    for (int axis{0}; axis < 3; ++axis)
    {
        EXPECT_FLOAT_EQ(tActual.position[axis], tExpected.position[axis]);
        EXPECT_FLOAT_EQ(tActual.rotation[axis], tExpected.rotation[axis]);
    }
    EXPECT_FLOAT_EQ(tActual.scale, tExpected.scale);
}

auto ExpectComponentEqual(
    EditorCameraControllerComponent const& tExpected,
    EditorCameraControllerComponent const& tActual) -> void
{
    EXPECT_FLOAT_EQ(tActual.moveSpeed, tExpected.moveSpeed);
    EXPECT_FLOAT_EQ(tActual.lookSensitivity, tExpected.lookSensitivity);
}

auto ExpectComponentEqual(MeshRendererComponent const& tExpected, MeshRendererComponent const& tActual) -> void
{
    EXPECT_EQ(tActual.meshHandle, tExpected.meshHandle);
    EXPECT_EQ(tActual.matHandle, tExpected.matHandle);
}

auto ExpectComponentEqual(LightComponent const& tExpected, LightComponent const& tActual) -> void
{
    EXPECT_EQ(tActual.color, tExpected.color);
    EXPECT_FLOAT_EQ(tActual.ambientFactor, tExpected.ambientFactor);
    EXPECT_FLOAT_EQ(tActual.specularFactor, tExpected.specularFactor);
}

template<Component T>
auto ExpectRestoredComponent(
    Scene&       tOriginalScene,
    EntityHandle tOriginalEntity,
    Scene&       tLoadedScene,
    EntityHandle tLoadedEntity) -> void
{
    if (!tOriginalScene.HasComponent(tOriginalEntity, T::kType))
    {
        return;
    }

    ASSERT_TRUE(tLoadedScene.HasComponent(tLoadedEntity, T::kType));

    auto const* expected = tOriginalScene.GetComponent<T>(tOriginalEntity);
    auto const* actual   = tLoadedScene.GetComponent<T>(tLoadedEntity);

    ASSERT_NE(expected, nullptr);
    ASSERT_NE(actual, nullptr);

    ExpectComponentEqual(*expected, *actual);
}
}// namespace

TEST_F(SceneSerializationTest, WritesEntityOrderAndRemapsChildrenAcrossRetiredSlots)
{
    auto const retired = scene->NewEntity();
    auto const gap     = scene->NewEntity();
    auto const parent  = scene->NewEntity(scene->sceneRoot, utils::RegisterString("Parent"));
    ASSERT_TRUE(scene->RetireEntity(gap));
    ASSERT_TRUE(scene->RetireEntity(retired));

    auto const child = scene->NewEntity(parent, utils::RegisterString("Child"));
    ASSERT_EQ(child.index, retired.index);
    ASSERT_NE(child.version, retired.version);
    ASSERT_LT(child.index, parent.index);
    ASSERT_NE(scene->AddComponent(child, CameraComponent{}), nullptr);
    ASSERT_NE(scene->AddComponent(child, TransformComponent{.position = {42.0F, 3.0F, -1.0F}}), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(child));

    auto const path = directory / "test.yaml";
    ASSERT_TRUE(Save(path));

    auto const document = YAML::LoadFile(path.string());
    auto const entities = document["scene"];
    ASSERT_EQ(entities.size(), 3U);
    EXPECT_EQ(entities[0]["info"]["children"].as<std::vector<EntityIndex>>(), (std::vector<EntityIndex>{2U}));
    EXPECT_EQ(entities[1]["info"]["debugName"].as<std::string>(), "Child");
    EXPECT_FALSE(entities[1]["info"]["graphHandle"]);
    EXPECT_EQ(entities[1]["transform"]["position"].as<std::vector<float>>(), (std::vector<float>{42.0F, 3.0F, -1.0F}));
    EXPECT_EQ(entities[2]["info"]["debugName"].as<std::string>(), "Parent");
    EXPECT_EQ(entities[2]["info"]["children"].as<std::vector<EntityIndex>>(), (std::vector<EntityIndex>{1U}));
    EXPECT_EQ(document["activeCamera"].as<EntityIndex>(), 1U);

    auto* loaded = manager->LoadScene(path.string());
    ASSERT_NE(loaded, nullptr);
    auto const restoredChild  = loaded->GetEntityAtIndex(1U);
    auto const restoredParent = loaded->GetEntityAtIndex(2U);
    EXPECT_TRUE(loaded->GetParent(restoredChild) == restoredParent);
    EXPECT_TRUE(loaded->GetChild(restoredParent) == restoredChild);
    EXPECT_TRUE(loaded->activeCamera == restoredChild);
}

TEST_F(SceneSerializationTest, WritesRootOnlyAndTruncatesExistingFile)
{
    auto const path = directory / "root.yaml";
    {
        std::ofstream old{path};
        old << std::string(4096, 'x');
    }

    ASSERT_TRUE(Save(path));

    auto const document = YAML::LoadFile(path.string());
    auto const entities = document["scene"];
    ASSERT_EQ(entities.size(), 1U);
    EXPECT_EQ(entities[0]["info"]["debugName"].as<std::string>(), "SceneRoot");
    EXPECT_EQ(entities[0]["info"]["children"].size(), 0U);
    EXPECT_TRUE(document["activeCamera"].IsNull());
    EXPECT_LT(std::filesystem::file_size(path), 4096U);
}

TEST_F(SceneSerializationTest, ReportsMissingSceneAndOpenFailure)
{
    SceneManager emptyManager;
    EXPECT_FALSE(emptyManager.SaveCurrentScene(utils::RegisterString((directory / "missing.yaml").string())));
    EXPECT_FALSE(std::filesystem::exists(directory / "missing.yaml"));
    EXPECT_FALSE(Save(directory / "absent" / "fail.yaml"));
    EXPECT_EQ(manager->LoadScene((directory / "missing.yaml").string()), nullptr);
    EXPECT_EQ(manager->GetCurrentScene(), scene);
}

TEST_F(SceneSerializationTest, DefaultEditorCameraRoundTripsWithoutDuplication)
{
    scene                 = SceneManager::Get().CreateScene();
    auto* transform       = scene->GetMutableComponent<TransformComponent>(scene->activeCamera);
    auto* controller      = scene->GetMutableComponent<EditorCameraControllerComponent>(scene->activeCamera);
    transform->position   = {1.0F, 2.0F, 3.0F};
    transform->rotation   = {-25.0F, 40.0F, 0.0F};
    controller->moveSpeed = 12.0F;
    auto const path       = directory / "editor-camera.yaml";
    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::Get().LoadScene(path.string());

    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(SceneManager::Get().GetCurrentScene(), loaded);
    EXPECT_EQ(loaded->GetEntityPoolSize(), 2U);
    ExpectChildren(*loaded, loaded->sceneRoot, {"Editor Camera"_sid});
    auto const camera = loaded->activeCamera;
    EXPECT_TRUE(camera == loaded->GetChild(loaded->sceneRoot));
    ExpectRestoredComponent<CameraComponent>(*scene, scene->activeCamera, *loaded, camera);
    ExpectRestoredComponent<TransformComponent>(*scene, scene->activeCamera, *loaded, camera);
    ExpectRestoredComponent<EditorCameraControllerComponent>(*scene, scene->activeCamera, *loaded, camera);
}

TEST_F(SceneSerializationTest, LoadPreservesSiblingOrderAndLinksAtEveryDepth)
{
    auto const a = scene->NewEntity(scene->sceneRoot, "A"_sid);
    scene->NewEntity(scene->sceneRoot, "B"_sid);
    scene->NewEntity(scene->sceneRoot, "C"_sid);
    scene->NewEntity(a, "D"_sid);

    auto const e = scene->NewEntity(a, "E"_sid);
    scene->NewEntity(e, "F"_sid);
    scene->NewEntity(e, "G"_sid);

    auto const path = directory / "order.yaml";

    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::Get().LoadScene(path.string());

    ASSERT_NE(loaded, nullptr);

    ExpectChildren(*loaded, loaded->sceneRoot, {"C"_sid, "B"_sid, "A"_sid});

    auto const loadedC = loaded->GetChild(loaded->sceneRoot);
    auto const loadedB = loaded->GetNextSibling(loadedC);
    auto const loadedA = loaded->GetNextSibling(loadedB);
    ExpectChildren(*loaded, loadedA, {"E"_sid, "D"_sid});

    auto const loadedE = loaded->GetChild(loadedA);
    ExpectChildren(*loaded, loadedE, {"G"_sid, "F"_sid});

    // Removing a middle sibling also exercises the restored previous-sibling link.
    ASSERT_TRUE(loaded->RetireEntity(loadedB));

    ExpectChildren(*loaded, loaded->sceneRoot, {"C"_sid, "A"_sid});

    ASSERT_TRUE(loaded->RetireEntity(loaded->GetChild(loadedA)));

    ExpectChildren(*loaded, loadedA, {"D"_sid});

    auto const added = loaded->NewEntity(loadedA, "H"_sid);

    ASSERT_TRUE(added != kInvalidEntityHandle);

    ExpectChildren(*loaded, loadedA, {"H"_sid, "D"_sid});
}

TEST_F(SceneSerializationTest, LoadPreservesRootOnlyScene)
{
    auto const path = directory / "root-only.yaml";

    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::Get().LoadScene(path.string());

    ASSERT_NE(loaded, nullptr);
    EXPECT_TRUE(loaded->GetChild(loaded->sceneRoot) == kInvalidEntityHandle);
    EXPECT_EQ(loaded->GetComponent<InfoComponent>(loaded->sceneRoot)->childCount, 0U);
    EXPECT_EQ(loaded->GetComponent<InfoComponent>(loaded->sceneRoot)->debugName, "SceneRoot"_sid);
}

TEST_F(SceneSerializationTest, LoadRestoresEveryImplementedComponentInEntityOrder)
{
    auto const a = scene->NewEntity(scene->sceneRoot, "A"_sid);
    auto const b = scene->NewEntity(scene->sceneRoot, "B"_sid);
    auto const c = scene->NewEntity(a, "C"_sid);

    CameraComponent const cameraB{
        .cameraType = CameraType::kOrtho,
        .nearZ      = 0.25F,
        .farZ       = 500.0F,
        .fov        = 75.0F,
        .size       = 22.0F,
    };
    CameraComponent const cameraC{
        .cameraType = CameraType::kPerspective,
        .nearZ      = 0.5F,
        .farZ       = 1000.0F,
        .fov        = 90.0F,
        .size       = 3.0F,
    };

    TransformComponent const transformA{
        .position = {1.0F, 2.0F, 3.0F},
        .rotation = {4.0F, 5.0F, 6.0F},
        .scale    = 7.0F,
    };
    TransformComponent const transformB{
        .position = {9.0F, 8.0F, 7.0F},
        .rotation = {6.0F, 5.0F, 4.0F},
        .scale    = 3.0F,
    };

    EditorCameraControllerComponent const controllerC{.moveSpeed = 17.0F, .lookSensitivity = 0.75F};

    MeshRendererComponent const meshB{rendering::kDefaultQuad, utils::RegisterString("material.Suzanne")};
    MeshRendererComponent const meshC{rendering::kDefaultCube};

    ASSERT_NE(scene->AddComponent(a, VisibilityComponent{.isVisible = false}), nullptr);
    ASSERT_NE(scene->AddComponent(c, VisibilityComponent{.isVisible = true}), nullptr);
    ASSERT_NE(scene->AddComponent(b, cameraB), nullptr);
    ASSERT_NE(scene->AddComponent(c, cameraC), nullptr);
    ASSERT_NE(scene->AddComponent(a, transformA), nullptr);
    ASSERT_NE(scene->AddComponent(b, transformB), nullptr);
    ASSERT_NE(scene->AddComponent(c, controllerC), nullptr);
    ASSERT_NE(scene->AddComponent(b, meshB), nullptr);
    ASSERT_NE(scene->AddComponent(c, meshC), nullptr);
    ASSERT_NE(scene->AddComponent(c, LightComponent{.color = {0.4F, 0.2F, 1.0F}, .ambientFactor = 0.5F}), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(c));

    auto const path = directory / "components.yaml";

    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::Get().LoadScene(path.string());

    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->activeCamera.index, 3U);
    EXPECT_EQ(loaded->activeCamera.version, 0U);

    std::array<EntityHandle, 3U> const originalsInEntityOrder{a, b, c};
    for (EntityIndex i{1U}; i <= originalsInEntityOrder.size(); ++i)
    {
        SCOPED_TRACE(i);
        auto const original     = originalsInEntityOrder[i - 1U];
        auto const restored     = loaded->GetEntityAtIndex(i);
        auto const originalMask = scene->GetEntityDataByIndex(original.index)->componentMask;
        EXPECT_EQ(loaded->GetEntityDataByIndex(i)->componentMask, originalMask);

        ExpectRestoredComponent<VisibilityComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<CameraComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<TransformComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<EditorCameraControllerComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<MeshRendererComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<LightComponent>(*scene, original, *loaded, restored);
    }
}

TEST_F(SceneSerializationTest, LoadsNamedMeshAndMaterialFromYaml)
{
    auto const path = directory / "handwritten.yaml";
    {
        std::ofstream file(path);
        file << R"(scene:
  - info: {debugName: SceneRoot, children: [1]}
  - info: {debugName: Suzanne}
    transform:
      position: [0.33, 22.222, 4.1254]
    mesh_renderer:
      mesh: mesh.Suzanne
      material: material.Suzanne
)";
    }

    auto* loaded = manager->LoadScene(path.string());
    ASSERT_NE(loaded, nullptr);
    auto const handle = loaded->GetEntityAtIndex(1U);
    auto const* mesh = loaded->GetComponent<MeshRendererComponent>(handle);
    ASSERT_NE(mesh, nullptr);
    EXPECT_EQ(mesh->meshHandle, "mesh.Suzanne"_sid);
    EXPECT_EQ(mesh->matHandle, "material.Suzanne"_sid);
    EXPECT_EQ(utils::GetStringFromID(mesh->meshHandle), "mesh.Suzanne");
    auto const* transform = loaded->GetComponent<TransformComponent>(handle);
    ASSERT_NE(transform, nullptr);
    EXPECT_EQ(transform->position, glm::vec3(0.33F, 22.222F, 4.1254F));
    EXPECT_FLOAT_EQ(transform->scale, 1.0F);
}

TEST_F(SceneSerializationTest, InvalidYamlOrHierarchyLeavesCurrentSceneUnchanged)
{
    auto const path = directory / "invalid.yaml";
    for (auto const yaml : {
             "scene: [",
             "scene: [{info: {debugName: Root, children: [1, 1]}}, {info: {debugName: Child}}]",
             "scene: [{info: {debugName: Root, children: [2]}}, {info: {debugName: Child}}]",
             "scene: [{info: {debugName: Root}}, {info: {debugName: Orphan}}]",
         })
    {
        SCOPED_TRACE(yaml);
        std::ofstream(path) << yaml;
        EXPECT_EQ(manager->LoadScene(path.string()), nullptr);
        EXPECT_EQ(manager->GetCurrentScene(), scene);
    }
}

TEST_F(SceneSerializationTest, LoadedViewsFollowSubtreeRetirementAndEntityReuse)
{
    auto const               branch   = scene->NewEntity(scene->sceneRoot, "Branch"_sid);
    auto const               camera   = scene->NewEntity(branch, "Camera"_sid);
    auto const               survivor = scene->NewEntity(scene->sceneRoot, "Survivor"_sid);
    CameraComponent const    branchCamera{.fov = 75.0F};
    CameraComponent const    survivorCamera{.fov = 65.0F};
    TransformComponent const transform{.position = {10.0F, 0.0F, 0.0F}};
    ASSERT_NE(scene->AddComponent(camera, branchCamera), nullptr);
    ASSERT_NE(scene->AddComponent(camera, transform), nullptr);
    ASSERT_NE(scene->AddComponent(survivor, survivorCamera), nullptr);
    ASSERT_NE(scene->AddComponent(survivor, transform), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(camera));
    auto const path = directory / "view-lifecycle.yaml";
    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::Get().LoadScene(path.string());
    ASSERT_NE(loaded, nullptr);
    auto const         loadedSurvivor = loaded->GetChild(loaded->sceneRoot);
    auto const         loadedBranch   = loaded->GetNextSibling(loadedSurvivor);
    auto const         oldCamera      = loaded->activeCamera;
    std::vector<float> observedFovs;

    for (auto [cameraComponent, transformComponent] : View<Read<CameraComponent>, Write<TransformComponent>>{loaded})
    {
        observedFovs.push_back(cameraComponent->fov);
        transformComponent->position.x += cameraComponent->fov;
    }

    std::vector<float> const expectedBeforeRemoval{75.0F, 65.0F};
    EXPECT_EQ(observedFovs, expectedBeforeRemoval);
    EXPECT_FLOAT_EQ(loaded->GetComponent<TransformComponent>(loadedSurvivor)->position.x, 75.0F);
    EXPECT_FLOAT_EQ(loaded->GetComponent<TransformComponent>(oldCamera)->position.x, 85.0F);

    ASSERT_TRUE(loaded->RetireEntity(loadedBranch));
    observedFovs.clear();
    for (auto [cameraComponent, transformComponent] : View<Read<CameraComponent, TransformComponent>>{loaded})
    {
        observedFovs.push_back(cameraComponent->fov);
        EXPECT_FLOAT_EQ(transformComponent->position.x, 75.0F);
    }

    std::vector<float> const expectedAfterRemoval{65.0F};
    EXPECT_EQ(observedFovs, expectedAfterRemoval);
    EXPECT_TRUE(loaded->activeCamera == kInvalidEntityHandle);
    EXPECT_EQ(loaded->GetComponent<CameraComponent>(oldCamera), nullptr);

    auto const replacement = loaded->NewEntity(loadedSurvivor, "Replacement"_sid);
    ASSERT_EQ(replacement.index, oldCamera.index);
    ASSERT_NE(replacement.version, oldCamera.version);
    EXPECT_FALSE(loaded->HasComponent(replacement, ComponentType::kCamera));
    ASSERT_NE(loaded->AddComponent(replacement, CameraComponent{.fov = 95.0F}), nullptr);
    ASSERT_NE(loaded->AddComponent(replacement, transform), nullptr);

    observedFovs.clear();
    for (auto [cameraComponent] : View<Read<CameraComponent>>{loaded})
    {
        observedFovs.push_back(cameraComponent->fov);
    }

    std::vector<float> const expectedAfterReuse{95.0F, 65.0F};
    EXPECT_EQ(observedFovs, expectedAfterReuse);
    EXPECT_EQ(loaded->GetComponent<CameraComponent>(oldCamera), nullptr);
}

TEST_F(SceneSerializationTest, EditedLoadedScenesCanBeSavedAndLoadedAgain)
{
    auto const               camera = scene->NewEntity(scene->sceneRoot, "Camera"_sid);
    CameraComponent const    originalCamera{.fov = 31.0F};
    TransformComponent const originalTransform{.position = {3.0F, 0.0F, 0.0F}};
    ASSERT_NE(scene->AddComponent(camera, originalCamera), nullptr);
    ASSERT_NE(scene->AddComponent(camera, originalTransform), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(camera));
    auto const firstPath = directory / "original.yaml";
    ASSERT_TRUE(Save(firstPath));
    auto* loaded = SceneManager::Get().LoadScene(firstPath.string());
    ASSERT_NE(loaded, nullptr);

    auto const loadedCamera    = loaded->activeCamera;
    auto*      cameraComponent = loaded->GetMutableComponent<CameraComponent>(loadedCamera);
    ASSERT_NE(cameraComponent, nullptr);
    cameraComponent->fov           = 48.0F;
    auto const               child = loaded->NewEntity(loadedCamera, "CreatedAfterLoad"_sid);
    TransformComponent const childTransform{.position = {17.0F, 0.0F, 0.0F}};
    ASSERT_NE(loaded->AddComponent(child, childTransform), nullptr);
    for (auto [transform] : View<Write<TransformComponent>>{loaded})
    {
        transform->position.x *= 2.0F;
    }
    auto const secondPath = directory / "edited.yaml";
    ASSERT_TRUE(Save(secondPath));

    auto* reloaded = SceneManager::Get().LoadScene(secondPath.string());

    ASSERT_NE(reloaded, nullptr);
    auto const reloadedCamera = reloaded->activeCamera;
    auto const reloadedChild  = reloaded->GetChild(reloadedCamera);
    ASSERT_TRUE(reloadedChild != kInvalidEntityHandle);
    auto const* restoredCamera = reloaded->GetComponent<CameraComponent>(reloadedCamera);
    ASSERT_NE(restoredCamera, nullptr);
    EXPECT_FLOAT_EQ(restoredCamera->fov, 48.0F);
    EXPECT_EQ(reloaded->GetComponent<InfoComponent>(reloadedChild)->debugName, "CreatedAfterLoad"_sid);
    EXPECT_TRUE(reloaded->GetParent(reloadedChild) == reloadedCamera);
    EXPECT_EQ(reloaded->GetComponent<InfoComponent>(reloadedCamera)->childCount, 1U);

    std::vector<float> positions;
    for (auto [transform] : View<Read<TransformComponent>>{reloaded})
    {
        positions.push_back(transform->position.x);
    }

    std::vector<float> const expectedPositions{6.0F, 34.0F};
    EXPECT_EQ(positions, expectedPositions);
}

}// namespace polos::ecs
