#include "polos/ecs/component_pool.hpp"
#include "polos/ecs/components/visibility_component.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <utility>

namespace polos::ecs
{
namespace
{
struct TrackedComponent
{
    static constexpr ComponentType kType = ComponentType::kVisibility;

    static inline std::size_t liveInstances{0U};

    TrackedComponent()
    {
        ++liveInstances;
    }

    ~TrackedComponent()
    {
        --liveInstances;
    }
};

TEST(ComponentPoolTest, InitializesIndependentSlotsAndCanBeRecreated)
{
    ComponentPool pool;
    pool.Create<VisibilityComponent>();

    ASSERT_TRUE(pool.IsInitialized());
    EXPECT_EQ(pool.elemSize, sizeof(VisibilityComponent));
    EXPECT_TRUE(pool.Get<VisibilityComponent>(0U)->isVisible);
    EXPECT_TRUE(pool.Get<VisibilityComponent>(Settings::kMaxEntities - 1U)->isVisible);

    pool.Get<VisibilityComponent>(0U)->isVisible = false;

    EXPECT_FALSE(pool.Get<VisibilityComponent>(0U)->isVisible);
    EXPECT_TRUE(pool.Get<VisibilityComponent>(1U)->isVisible);

    pool.Destroy();
    pool.Destroy();

    EXPECT_FALSE(pool.IsInitialized());
    EXPECT_EQ(pool.elemSize, 0U);
    EXPECT_EQ(pool.GetTotalComponents(), 0U);

    pool.Create<VisibilityComponent>();

    ASSERT_TRUE(pool.IsInitialized());
    EXPECT_TRUE(pool.Get<VisibilityComponent>(0U)->isVisible);
}

TEST(ComponentPoolTest, MoveConstructionTransfersOwnershipAndPreservesAddresses)
{
    ASSERT_EQ(TrackedComponent::liveInstances, 0U);

    {
        ComponentPool source;
        source.Create<TrackedComponent>();
        auto const* originalSlot = source.Get<TrackedComponent>(17U);

        ComponentPool destination{std::move(source)};
        source.Destroy();

        EXPECT_FALSE(source.IsInitialized());
        ASSERT_TRUE(destination.IsInitialized());
        EXPECT_EQ(destination.Get<TrackedComponent>(17U), originalSlot);
        EXPECT_EQ(TrackedComponent::liveInstances, Settings::kMaxEntities);
    }

    EXPECT_EQ(TrackedComponent::liveInstances, 0U);
}

TEST(ComponentPoolTest, MoveAssignmentReleasesOldStorageAndTransfersOwnership)
{
    ASSERT_EQ(TrackedComponent::liveInstances, 0U);

    {
        ComponentPool source;
        ComponentPool destination;
        source.Create<TrackedComponent>();
        destination.Create<TrackedComponent>();
        auto const* originalSlot = source.Get<TrackedComponent>(23U);
        ASSERT_EQ(TrackedComponent::liveInstances, 2U * Settings::kMaxEntities);

        destination = std::move(source);
        source.Destroy();

        EXPECT_FALSE(source.IsInitialized());
        ASSERT_TRUE(destination.IsInitialized());
        EXPECT_EQ(destination.Get<TrackedComponent>(23U), originalSlot);
        EXPECT_EQ(TrackedComponent::liveInstances, Settings::kMaxEntities);
    }

    EXPECT_EQ(TrackedComponent::liveInstances, 0U);
}

}// namespace
}// namespace polos::ecs
