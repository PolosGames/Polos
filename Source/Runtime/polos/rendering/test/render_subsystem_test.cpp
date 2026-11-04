///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/pipeline_cache.hpp"
#include "polos/rendering/cache/shader_cache.hpp"
#include "polos/rendering/cache/texture_cache.hpp"
#include "polos/rendering/render_subsystem.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vk/vulkan_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_swapchain.hpp"

#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <vector>

namespace polos::rendering
{
namespace
{
static_assert(RenderSubsystem<VulkanContext>);
static_assert(RenderSubsystem<VulkanDevice>);
static_assert(RenderSubsystem<VulkanSwapchain>);
static_assert(RenderSubsystem<ShaderCache>);
static_assert(RenderSubsystem<PipelineCache>);
static_assert(RenderSubsystem<TextureCache>);
static_assert(!RenderSubsystem<int>);

struct WrongCreateResult
{
    struct CreationDetails
    {
    };

    auto Create(CreationDetails const&) -> bool;
    auto Destroy() -> void;
};

struct WrongDestroyResult
{
    struct CreationDetails
    {
    };

    auto Create(CreationDetails const&) -> Result<void>;
    auto Destroy() -> Result<void>;
};

static_assert(!RenderSubsystem<WrongCreateResult>);
static_assert(!RenderSubsystem<WrongDestroyResult>);

struct TestSubsystem
{
    struct CreationDetails
    {
        bool fail{false};
    };

    int               id;
    std::vector<int>& events;
    bool              live{false};

    auto Create(CreationDetails const& tDetails) -> Result<void>
    {
        events.push_back(id);
        live = true;
        if (tDetails.fail)
        {
            return ErrorType{RenderingErrc::kFailedCreateShaderModule};
        }
        return {};
    }

    auto Destroy() -> void
    {
        if (live)
        {
            events.push_back(-id);
            live = false;
        }
    }
};

auto ExpectedRollbackEvents(std::size_t tFailureIndex) -> std::vector<int>
{
    std::vector<int> events;
    for (std::size_t i{0U}; i <= tFailureIndex; ++i)
    {
        events.push_back(static_cast<int>(i + 1U));
    }

    for (std::size_t i{tFailureIndex + 1U}; i > 0U; --i)
    {
        events.push_back(-static_cast<int>(i));
    }

    return events;
}

TEST(RenderSubsystemTest, DetailsAreBuiltWhenDependenciesAreReady)
{
    std::vector<int> events;
    TestSubsystem    first{1, events};
    TestSubsystem    second{2, events};
    bool             detailsBuilt{false};
    std::array       entries{
        RegisterSubsystem(
            first,
            [] {
                return TestSubsystem::CreationDetails{};
            }),
        RegisterSubsystem(second, [&] {
            detailsBuilt = true;
            EXPECT_TRUE(first.live);
            return TestSubsystem::CreationDetails{};
        }),
    };

    EXPECT_FALSE(detailsBuilt);

    std::size_t initialized{0U};

    ASSERT_TRUE(CreateSubsystems(entries, initialized));
    EXPECT_TRUE(detailsBuilt);
    EXPECT_EQ(initialized, 2U);

    DestroySubsystems(entries, initialized);
    std::vector<int> const expectedEvents{1, 2, -2, -1};

    EXPECT_EQ(events, expectedEvents);
    EXPECT_EQ(initialized, 0U);
}

TEST(RenderSubsystemTest, FailureCleansPartialStateAndRollsBackEarlierEntries)
{
    // Exercise every failure position, including failure before any entry succeeds.
    for (std::size_t failure{0U}; failure < 3U; ++failure)
    {
        SCOPED_TRACE(failure);

        std::vector<int> events;
        TestSubsystem    first{1, events};
        TestSubsystem    second{2, events};
        TestSubsystem    third{3, events};
        std::array       entries{
            RegisterSubsystem(
                first,
                [failure] {
                    return TestSubsystem::CreationDetails{failure == 0U};
                }),
            RegisterSubsystem(
                second,
                [failure] {
                    return TestSubsystem::CreationDetails{failure == 1U};
                }),
            RegisterSubsystem(third, [failure] {
                return TestSubsystem::CreationDetails{failure == 2U};
            }),
        };

        std::size_t initialized{0U};
        auto        result = CreateSubsystems(entries, initialized);
        ASSERT_FALSE(result);
        EXPECT_TRUE(result.error() == MakeErrorCode(RenderingErrc::kFailedCreateShaderModule));
        EXPECT_EQ(initialized, 0U);
        EXPECT_FALSE(first.live);
        EXPECT_FALSE(second.live);
        EXPECT_FALSE(third.live);

        auto const expected = ExpectedRollbackEvents(failure);
        EXPECT_EQ(events, expected);

        DestroySubsystems(entries, initialized);
        EXPECT_EQ(events, expected);
    }
}

TEST(RenderSubsystemTest, SupportsMoveOnlyFactoriesAndRepeatedLifecycle)
{
    std::vector<int> events;
    TestSubsystem    subsystem{1, events};
    std::array       entries{
        RegisterSubsystem(subsystem, [details = std::make_unique<TestSubsystem::CreationDetails>()] {
            return *details;
        }),
    };

    std::size_t initialized{0U};

    ASSERT_TRUE(CreateSubsystems(entries, initialized));
    ASSERT_TRUE(CreateSubsystems(entries, initialized));

    DestroySubsystems(entries, initialized);
    DestroySubsystems(entries, initialized);

    ASSERT_TRUE(CreateSubsystems(entries, initialized));

    DestroySubsystems(entries, initialized);

    std::vector<int> const expectedEvents{1, -1, 1, -1};

    EXPECT_EQ(events, expectedEvents);
}

TEST(RenderSubsystemTest, CanRetryAfterFailure)
{
    std::vector<int> events;
    TestSubsystem    subsystem{1, events};
    bool             fail{true};
    std::array       entries{
        RegisterSubsystem(subsystem, [&] {
            return TestSubsystem::CreationDetails{fail};
        }),
    };

    std::size_t initialized{0U};

    ASSERT_FALSE(CreateSubsystems(entries, initialized));

    fail = false;

    ASSERT_TRUE(CreateSubsystems(entries, initialized));

    DestroySubsystems(entries, initialized);

    std::vector<int> const expectedEvents{1, -1, 1, -1};

    EXPECT_EQ(events, expectedEvents);
}

TEST(RenderSubsystemTest, EmptyRegistrySucceeds)
{
    std::array<SubsystemEntry, 0U> entries;
    std::size_t                    initialized{0U};

    EXPECT_TRUE(CreateSubsystems(entries, initialized));

    DestroySubsystems(entries, initialized);

    EXPECT_EQ(initialized, 0U);
}

}// namespace
}// namespace polos::rendering
