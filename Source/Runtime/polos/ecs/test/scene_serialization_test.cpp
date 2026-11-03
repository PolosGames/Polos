#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/editor_camera_controller_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/quad_renderer_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/components/visibility_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/scene_manager.hpp"
#include "polos/utils/string_map.hpp"

#include <gtest/gtest.h>

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

namespace polos::ecs
{
class SceneSerializationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        manager                 = std::make_unique<SceneManager>();
        SceneManager::sInstance = manager.get();

        auto owned = std::make_unique<Scene>("SerializationTest"_sid);
        scene      = owned.get();
        manager->mScenes.push_back(std::move(owned));

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
        scene->name = utils::RegisterString(path.string());
        return SceneManager::SaveScene(scene->name);
    }

    std::unique_ptr<SceneManager> manager;
    Scene*                        scene{};
    std::filesystem::path         directory;
};

namespace
{
struct ComponentHeader
{
    std::uint8_t  type;
    std::uint64_t end;
};

struct Header
{
    std::uint64_t                               entitiesEnd;
    std::array<ComponentHeader, kMaxComponents> components;
};

template<typename T>
auto Read(std::ifstream& file) -> T
{
    T value{};

    EXPECT_TRUE(static_cast<bool>(file.read(reinterpret_cast<char*>(&value), sizeof(value))));

    return value;
}

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
        EXPECT_FLOAT_EQ(tActual.scale[axis], tExpected.scale[axis]);
    }
}

auto ExpectComponentEqual(
    EditorCameraControllerComponent const& tExpected,
    EditorCameraControllerComponent const& tActual) -> void
{
    EXPECT_FLOAT_EQ(tActual.moveSpeed, tExpected.moveSpeed);
    EXPECT_FLOAT_EQ(tActual.lookSensitivity, tExpected.lookSensitivity);
}

auto ExpectComponentEqual(QuadRendererComponent const& tExpected, QuadRendererComponent const& tActual) -> void
{
    for (int axis{0}; axis < 4; ++axis)
    {
        EXPECT_FLOAT_EQ(tActual.color[axis], tExpected.color[axis]);
    }

    for (int axis{0}; axis < 2; ++axis)
    {
        EXPECT_FLOAT_EQ(tActual.scissor[axis], tExpected.scissor[axis]);
    }

    EXPECT_EQ(tActual.textureName, tExpected.textureName);
    EXPECT_FLOAT_EQ(tActual.tilingFactor, tExpected.tilingFactor);
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

TEST_F(SceneSerializationTest, WritesDfsRecordsSparseComponentsAndCamera)
{
    auto const a       = scene->NewEntity();
    auto const b       = scene->NewEntity();
    auto const removed = scene->NewEntity(a);
    auto const e       = scene->NewEntity(b);

    ASSERT_TRUE(scene->RetireEntity(removed));

    auto const f = scene->NewEntity(a);

    ASSERT_EQ(f.index, removed.index);
    ASSERT_NE(f.version, removed.version);

    TransformComponent const transformB{.position = {42.0F, 0.0F, 0.0F}};
    TransformComponent const transformF{.position = {91.0F, 0.0F, 0.0F}};

    ASSERT_NE(scene->AddComponent(b, transformB), nullptr);
    ASSERT_NE(scene->AddComponent(f, transformF), nullptr);
    ASSERT_NE(scene->AddComponent(f, CameraComponent{}), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(f));

    auto const path = directory / "test.scene";

    ASSERT_TRUE(Save(path));

    std::ifstream file{path, std::ios::binary};

    ASSERT_TRUE(file.is_open());

    auto const header = Read<Header>(file);

    EXPECT_EQ(Read<utils::string_id>(file), scene->name);
    EXPECT_EQ(Read<EntityIndex>(file), 4U);

    constexpr std::size_t count = 5U;

    EXPECT_EQ(
        header.entitiesEnd,
        sizeof(Header) + sizeof(utils::string_id) + sizeof(EntityIndex) + count * sizeof(SerializedEntity));

    std::array<EntityHandle, count> const     order{scene->sceneRoot, b, e, a, f};
    std::array<std::uint32_t, kMaxComponents> componentCounts{};

    for (std::size_t i{0U}; i < count; ++i)
    {
        SCOPED_TRACE(i);
        auto const  record = Read<SerializedEntity>(file);
        auto const* entity = scene->GetEntityDataByIndex(order[i].index);
        EXPECT_EQ(record.componentTypes, entity->componentMask.to_ullong());
        EXPECT_EQ(record.componentCount, entity->componentMask.count());

        for (std::size_t type{0U}; type < kMaxComponents; ++type)
        {
            if (entity->componentMask.test(type))
            {
                EXPECT_EQ(record.componentIndices[type], componentCounts[type]);
                ++componentCounts[type];
            }
        }
    }

    std::array<std::size_t, kMaxComponents> componentSizes{};
    componentSizes[static_cast<std::size_t>(ComponentType::kInfo)]      = sizeof(InfoComponent);
    componentSizes[static_cast<std::size_t>(ComponentType::kCamera)]    = sizeof(CameraComponent);
    componentSizes[static_cast<std::size_t>(ComponentType::kTransform)] = sizeof(TransformComponent);

    std::uint64_t sectionStart = header.entitiesEnd;
    for (std::size_t type{0U}; type < kMaxComponents; ++type)
    {
        SCOPED_TRACE(type);
        EXPECT_EQ(header.components[type].type, type);
        EXPECT_EQ(header.components[type].end - sectionStart, componentCounts[type] * componentSizes[type]);

        if (type == static_cast<std::size_t>(ComponentType::kInfo))
        {
            file.seekg(static_cast<std::streamoff>(sectionStart));
            for (auto handle : order)
            {
                auto const info = Read<InfoComponent>(file);
                EXPECT_EQ(info.graphHandle, scene->GetComponent<InfoComponent>(handle)->graphHandle);
                EXPECT_EQ(info.childCount, scene->GetComponent<InfoComponent>(handle)->childCount);
            }
        }

        if (type == static_cast<std::size_t>(ComponentType::kTransform))
        {
            file.seekg(static_cast<std::streamoff>(sectionStart));
            EXPECT_FLOAT_EQ(Read<TransformComponent>(file).position.x, 42.0F);
            EXPECT_FLOAT_EQ(Read<TransformComponent>(file).position.x, 91.0F);
        }

        sectionStart = header.components[type].end;
    }

    EXPECT_TRUE(file.good());
    EXPECT_EQ(std::filesystem::file_size(path), sectionStart);
}

TEST_F(SceneSerializationTest, WritesRootOnlyAndTruncatesExistingFile)
{
    auto const path = directory / "root.scene";
    {
        std::ofstream old{path};
        old << std::string(4096, 'x');
    }

    ASSERT_TRUE(Save(path));

    std::ifstream file{path, std::ios::binary};

    ASSERT_TRUE(file.is_open());

    auto const header = Read<Header>(file);

    EXPECT_EQ(Read<utils::string_id>(file), scene->name);
    EXPECT_EQ(Read<EntityIndex>(file), kInvalidEntityIndex);

    auto const root = Read<SerializedEntity>(file);

    EXPECT_EQ(root.componentCount, 1U);
    EXPECT_EQ(root.componentIndices[static_cast<std::size_t>(ComponentType::kInfo)], 0U);
    EXPECT_EQ(std::filesystem::file_size(path), header.components.back().end);
    EXPECT_LT(std::filesystem::file_size(path), 4096U);
}

TEST_F(SceneSerializationTest, ReportsMissingSceneAndOpenFailure)
{
    EXPECT_FALSE(SceneManager::SaveScene("MissingScene"_sid));
    EXPECT_FALSE(std::filesystem::exists(directory / "missing.scene"));
    EXPECT_FALSE(Save(directory / "absent" / "fail.scene"));
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

    auto const path = directory / "order.scene";

    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::LoadScene(path.string());

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
    auto const path = directory / "root-only.scene";

    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::LoadScene(path.string());

    ASSERT_NE(loaded, nullptr);
    EXPECT_TRUE(loaded->GetChild(loaded->sceneRoot) == kInvalidEntityHandle);
    EXPECT_EQ(loaded->GetComponent<InfoComponent>(loaded->sceneRoot)->childCount, 0U);
    EXPECT_EQ(loaded->GetComponent<InfoComponent>(loaded->sceneRoot)->debugName, "SceneRoot"_sid);
}

TEST_F(SceneSerializationTest, LoadRestoresEveryImplementedComponentInDfsOrder)
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
        .scale    = {7.0F, 8.0F, 9.0F},
    };
    TransformComponent const transformB{
        .position = {9.0F, 8.0F, 7.0F},
        .rotation = {6.0F, 5.0F, 4.0F},
        .scale    = {3.0F, 2.0F, 1.0F},
    };

    EditorCameraControllerComponent const controllerC{.moveSpeed = 17.0F, .lookSensitivity = 0.75F};

    QuadRendererComponent const quadB{
        .color        = {0.1F, 0.2F, 0.3F, 0.4F},
        .scissor      = {33.0F, 44.0F},
        .textureName  = "TextureB"_sid,
        .tilingFactor = 2.5F,
    };
    QuadRendererComponent const quadC{
        .color        = {0.9F, 0.8F, 0.7F, 0.6F},
        .scissor      = {55.0F, 66.0F},
        .textureName  = "TextureC"_sid,
        .tilingFactor = 4.5F,
    };

    ASSERT_NE(scene->AddComponent(a, VisibilityComponent{.isVisible = false}), nullptr);
    ASSERT_NE(scene->AddComponent(c, VisibilityComponent{.isVisible = true}), nullptr);
    ASSERT_NE(scene->AddComponent(b, cameraB), nullptr);
    ASSERT_NE(scene->AddComponent(c, cameraC), nullptr);
    ASSERT_NE(scene->AddComponent(a, transformA), nullptr);
    ASSERT_NE(scene->AddComponent(b, transformB), nullptr);
    ASSERT_NE(scene->AddComponent(c, controllerC), nullptr);
    ASSERT_NE(scene->AddComponent(b, quadB), nullptr);
    ASSERT_NE(scene->AddComponent(c, quadC), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(c));

    auto const path = directory / "components.scene";

    ASSERT_TRUE(Save(path));

    auto* loaded = SceneManager::LoadScene(path.string());

    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->activeCamera.index, 3U);
    EXPECT_EQ(loaded->activeCamera.version, 0U);

    std::array<EntityHandle, 3U> const originalsInDfsOrder{b, a, c};
    for (EntityIndex i{1U}; i <= originalsInDfsOrder.size(); ++i)
    {
        SCOPED_TRACE(i);
        auto const original     = originalsInDfsOrder[i - 1U];
        auto const restored     = loaded->GetEntityAtIndex(i);
        auto const originalMask = scene->GetEntityDataByIndex(original.index)->componentMask;
        EXPECT_EQ(loaded->GetEntityDataByIndex(i)->componentMask, originalMask);

        ExpectRestoredComponent<VisibilityComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<CameraComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<TransformComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<EditorCameraControllerComponent>(*scene, original, *loaded, restored);
        ExpectRestoredComponent<QuadRendererComponent>(*scene, original, *loaded, restored);
    }
}

}// namespace polos::ecs
