#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/scene.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <random>
#include <vector>

namespace polos::ecs
{
namespace
{
auto ExpectEntity(EntityHandle const tActual, EntityHandle const tExpected) -> void
{
    EXPECT_EQ(tActual.index, tExpected.index);
    EXPECT_EQ(tActual.version, tExpected.version);
}

TEST(SceneGraph, LinksAndMiddleSiblingRemoval)
{
    SceneGraph         graph;
    EntityHandle const a{1U, 3U};
    EntityHandle const b{2U, 4U};
    EntityHandle const c{3U, 5U};
    auto const         handleA = graph.AddChild(0U, a);
    auto const         handleB = graph.AddChild(0U, b);
    auto const         handleC = graph.AddChild(0U, c);
    ExpectEntity(graph.GetChild(0U), c);
    ExpectEntity(graph.GetNextSibling(handleC), b);
    ExpectEntity(graph.GetNextSibling(handleB), a);
    ExpectEntity(graph.GetParent(handleA), (EntityHandle{0U, 0U}));

    EXPECT_EQ(graph.RetireNode(handleB), 1U);

    ExpectEntity(graph.GetNextSibling(handleC), a);

    EXPECT_EQ(graph.RetireNode(handleC), 1U);

    ExpectEntity(graph.GetChild(0U), a);

    EXPECT_EQ(graph.RetireNode(handleA), 1U);

    ExpectEntity(graph.GetChild(0U), kInvalidEntityHandle);
}

TEST(SceneGraph, SubtreeRemovalAndStaleHandles)
{
    SceneGraph graph;
    auto const a = graph.AddChild(0U, {1U, 0U});
    auto const b = graph.AddChild(a, {2U, 0U});

    ASSERT_NE(graph.AddChild(b, {3U, 0U}), kInvalidSceneGraphHandle);

    auto const sibling = graph.AddChild(0U, {4U, 0U});

    EXPECT_EQ(graph.RetireNode(a), 3U);

    auto const reused = graph.AddChild(0U, {5U, 7U});

    EXPECT_NE(reused, a);
    EXPECT_EQ(graph.RetireNode(a), 0U);
    EXPECT_EQ(graph.AddChild(a, {6U, 0U}), kInvalidSceneGraphHandle);

    ExpectEntity(graph.GetParent(a), kInvalidEntityHandle);
    ExpectEntity(graph.GetChild(b), kInvalidEntityHandle);
    ExpectEntity(graph.GetParent(sibling), (EntityHandle{0U, 0U}));

    EXPECT_TRUE(graph.RetireChild(0U));

    ExpectEntity(graph.GetChild(0U), (EntityHandle{4U, 0U}));

    EXPECT_EQ(graph.RetireAllChildren(0U), 1U);
    EXPECT_FALSE(graph.RetireChild(0U));
    EXPECT_EQ(graph.RetireNode(0U), 0U);
    EXPECT_EQ(graph.RetireAllChildren(kInvalidSceneGraphHandle), 0U);
    EXPECT_EQ(graph.AddChild(0U, kInvalidEntityHandle), kInvalidSceneGraphHandle);

    ExpectEntity(graph.GetNextSibling(kInvalidSceneGraphHandle), kInvalidEntityHandle);
}

TEST(SceneGraph, FullCapacityDeepTreeAndReuse)
{
    SceneGraph       graph;
    SceneGraphHandle parent = 0U;
    for (EntityIndex i = 1U; i < Settings::kMaxEntities; ++i)
    {
        parent = graph.AddChild(parent, {i, 0U});
        ASSERT_NE(parent, kInvalidSceneGraphHandle);
    }

    EXPECT_EQ(graph.AddChild(0U, {1U, 0U}), kInvalidSceneGraphHandle);
    EXPECT_EQ(graph.RetireAllChildren(0U), Settings::kMaxEntities - 1U);
    EXPECT_EQ(graph.RetireAllChildren(0U), 0U);
    EXPECT_NE(graph.AddChild(0U, {1U, 1U}), kInvalidSceneGraphHandle);
}

TEST(SceneGraph, RandomTreeMatchesParentModel)
{
    SceneGraph                    graph;
    std::mt19937                  random{42U};
    constexpr EntityIndex         count = 4096U;
    std::vector<SceneGraphHandle> handles{0U};
    std::vector<EntityIndex>      parents{0U};
    std::vector<bool>             alive(count, true);
    for (EntityIndex i = 1U; i < count; ++i)
    {
        auto const parent = random() % i;
        parents.push_back(parent);
        handles.push_back(graph.AddChild(handles[parent], {i, 0U}));
    }

    for (EntityIndex iteration = 0U; iteration < 256U; ++iteration)
    {
        SCOPED_TRACE(iteration);

        auto const    root     = 1U + random() % (count - 1U);
        std::uint32_t expected = 0U;
        if (alive[root])
        {
            for (EntityIndex i = root; i < count; ++i)
            {
                auto ancestor = i;
                while (ancestor > root)
                {
                    ancestor = parents[ancestor];
                }
                if (ancestor == root && alive[i])
                {
                    alive[i] = false;
                    ++expected;
                }
            }
        }
        ASSERT_EQ(graph.RetireNode(handles[root]), expected);
    }

    for (EntityIndex parent = 0U; parent < count; ++parent)
    {
        SCOPED_TRACE(parent);

        if (!alive[parent])
        {
            continue;
        }
        auto child = graph.GetChild(handles[parent]);
        for (EntityIndex i = count - 1U; i > parent; --i)
        {
            if (alive[i] && parents[i] == parent)
            {
                ExpectEntity(child, (EntityHandle{i, 0U}));
                child = graph.GetNextSibling(handles[i]);
            }
        }
        ExpectEntity(child, kInvalidEntityHandle);
    }
}

TEST(SceneHierarchy, RetiresDescendantsAndReusesEntities)
{
    auto       scene   = std::make_unique<Scene>();
    auto const a       = scene->NewEntity();
    auto const b       = scene->NewEntity(a);
    auto const c       = scene->NewEntity(b);
    auto const sibling = scene->NewEntity();
    ExpectEntity(scene->GetParent(a), scene->sceneRoot);
    ExpectEntity(scene->GetChild(a), b);
    ExpectEntity(scene->GetNextSibling(sibling), a);

    EXPECT_FALSE(scene->RemoveComponent(a, ComponentType::kInfo));
    ASSERT_NE(scene->AddComponent(c, CameraComponent{}), nullptr);
    ASSERT_TRUE(scene->SetActiveCamera(c));
    EXPECT_TRUE(scene->RetireEntity(a));

    ExpectEntity(scene->activeCamera, kInvalidEntityHandle);
    ExpectEntity(scene->GetChild(scene->sceneRoot), sibling);
    for (auto const handle : {a, b, c})
    {
        SCOPED_TRACE(handle.index);

        auto const* data = scene->GetEntityDataByIndex(handle.index);
        ASSERT_NE(data, nullptr);
        EXPECT_EQ(data->handle.index, kInvalidEntityIndex);
        EXPECT_TRUE(data->componentMask.none());
        ExpectEntity(scene->GetParent(handle), kInvalidEntityHandle);
    }

    auto const reused = scene->NewEntity();

    EXPECT_EQ(reused.index, c.index);
    EXPECT_NE(reused.version, c.version);

    ExpectEntity(scene->GetParent(reused), scene->sceneRoot);

    EXPECT_FALSE(scene->RetireEntity(scene->sceneRoot));

    ExpectEntity(scene->GetParent(scene->sceneRoot), kInvalidEntityHandle);
}
}// namespace
}// namespace polos::ecs
