///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/pipeline_cache.hpp"

#include <gtest/gtest.h>

namespace polos::rendering::test
{

class PipelineCacheTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Pipeline cache tests would require mock Vulkan device
        // For now, test the logic that doesn't depend on Vulkan
    }

    void TearDown() override {}
};

TEST_F(PipelineCacheTest, InitialCacheIsEmpty)
{
    PipelineCache cache;

    auto result = cache.GetPipeline("nonexistent");
    EXPECT_FALSE(result.has_value());
}

TEST_F(PipelineCacheTest, TryGetNonExistentPipeline)
{
    PipelineCache cache;

    auto result = cache.GetPipeline("test_pipeline");
    EXPECT_FALSE(result.has_value());

    auto result2 = cache.GetPipeline(utils::StrHash64("test_pipeline"));
    EXPECT_FALSE(result2.has_value());
}

}// namespace polos::rendering::test
