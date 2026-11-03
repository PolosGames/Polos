#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/visibility_component.hpp"
#include "polos/ecs/view.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <tuple>
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

TEST(ViewTest, FiltersByAllRequestedComponentsAndReturnsLiveComponentPointers)
{
    auto                      scene            = std::make_unique<Scene>();
    auto const                cameraOnly       = scene->NewEntity();
    auto const                match            = scene->NewEntity();
    auto const                removedComponent = scene->NewEntity();
    auto const                retired          = scene->NewEntity();
    CameraComponent const     camera{.fov = 82.0F};
    VisibilityComponent const hidden{.isVisible = false};
    ASSERT_NE(scene->AddComponent(cameraOnly, camera), nullptr);
    for (auto entity : {match, removedComponent, retired})
    {
        SCOPED_TRACE(entity.index);
        ASSERT_NE(scene->AddComponent(entity, camera), nullptr);
        ASSERT_NE(scene->AddComponent(entity, hidden), nullptr);
    }
    ASSERT_TRUE(scene->RemoveComponent(removedComponent, ComponentType::kVisibility));
    ASSERT_TRUE(scene->RetireEntity(retired));

    View<VisibilityComponent, CameraComponent> const view{scene.get()};
    auto                                             iterator = view.begin();

    ASSERT_NE(iterator, view.end());
    auto const [visibility, actualCamera] = *iterator;
    EXPECT_EQ(visibility, scene->GetComponent<VisibilityComponent>(match));
    EXPECT_EQ(actualCamera, scene->GetComponent<CameraComponent>(match));
    EXPECT_FALSE(visibility->isVisible);
    EXPECT_FLOAT_EQ(actualCamera->fov, camera.fov);

    actualCamera->fov = 95.0F;
    ++iterator;

    EXPECT_EQ(iterator, view.end());
    EXPECT_FLOAT_EQ(scene->GetComponent<CameraComponent>(match)->fov, 95.0F);
}

TEST(ViewTest, UnfilteredViewSkipsRetiredSlotsAndExposesRecycledVersions)
{
    auto       scene    = std::make_unique<Scene>();
    auto const retired  = scene->NewEntity();
    auto const survivor = scene->NewEntity();
    ASSERT_TRUE(scene->RetireEntity(retired));
    View<> const view{scene.get()};

    std::vector<EntityHandle> beforeReuse;
    for (auto entity : view)
    {
        beforeReuse.push_back(entity);
    }

    ASSERT_EQ(beforeReuse.size(), 2U);
    ExpectEntity(beforeReuse[0], scene->sceneRoot);
    ExpectEntity(beforeReuse[1], survivor);

    auto const                replacement = scene->NewEntity();
    std::vector<EntityHandle> afterReuse;
    for (auto entity : view)
    {
        afterReuse.push_back(entity);
    }

    ASSERT_EQ(afterReuse.size(), 3U);
    ExpectEntity(afterReuse[0], scene->sceneRoot);
    ExpectEntity(afterReuse[1], replacement);
    ExpectEntity(afterReuse[2], survivor);
    EXPECT_EQ(replacement.index, retired.index);
    EXPECT_NE(replacement.version, retired.version);
}

TEST(ViewTest, IteratorsSupportEmptyRangesCopiesAndPostIncrement)
{
    auto                        scene = std::make_unique<Scene>();
    View<CameraComponent> const emptyView{scene.get()};

    EXPECT_EQ(emptyView.begin(), emptyView.end());
    EXPECT_EQ(emptyView.cbegin(), emptyView.cend());

    auto const   first  = scene->NewEntity();
    auto const   second = scene->NewEntity();
    View<> const view{scene.get()};
    auto         iterator        = view.cbegin();
    auto         independentCopy = iterator;

    auto previous = iterator++;

    ExpectEntity(*previous, scene->sceneRoot);
    ExpectEntity(*independentCopy, scene->sceneRoot);
    ExpectEntity(*iterator, first);

    ++iterator;

    ExpectEntity(*iterator, second);

    ++iterator;

    EXPECT_EQ(iterator, view.cend());
}

}// namespace
}// namespace polos::ecs
