///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/common.hpp"

#include <gtest/gtest.h>

namespace polos::rendering::test
{

TEST(RenderGraphResourceHandleTest, CreateAndDecompose)
{
    constexpr std::uint16_t test_index   = 44U;
    constexpr std::uint16_t test_version = 7U;

    auto handle = RenderGraphResourceHandle::Create(test_index, RenderGraphResourceType::kTexture, test_version);

    EXPECT_EQ(handle.Index(), test_index);
    EXPECT_EQ(handle.Type(), RenderGraphResourceType::kTexture);
    EXPECT_EQ(handle.Version(), test_version);
    EXPECT_TRUE(handle.IsValid());
}

TEST(RenderGraphResourceHandleTest, InvalidHandle)
{
    auto invalid_handle = RenderGraphResourceHandle::Invalid();

    EXPECT_FALSE(invalid_handle.IsValid());
    EXPECT_EQ(invalid_handle.Index(), 0U);
}

TEST(RenderGraphResourceHandleTest, DifferentTypesNotEqual)
{
    constexpr std::uint16_t test_index   = 10U;
    constexpr std::uint16_t test_version = 1U;

    auto texture_handle =
        RenderGraphResourceHandle::Create(test_index, RenderGraphResourceType::kTexture, test_version);
    auto buffer_handle = RenderGraphResourceHandle::Create(test_index, RenderGraphResourceType::kBuffer, test_version);

    EXPECT_EQ(texture_handle.Type(), RenderGraphResourceType::kTexture);
    EXPECT_EQ(buffer_handle.Type(), RenderGraphResourceType::kBuffer);
    EXPECT_NE(texture_handle, buffer_handle);
}

TEST(RenderGraphResourceHandleTest, MaxValues)
{
    constexpr std::uint16_t max_index   = 0xFFFF;
    constexpr std::uint16_t max_version = (1U << 12U) - 1U;// 12 bits for version

    auto handle = RenderGraphResourceHandle::Create(max_index, RenderGraphResourceType::kTexture, max_version);

    EXPECT_EQ(handle.Index(), max_index);
    EXPECT_EQ(handle.Version(), max_version);
}

}// namespace polos::rendering::test
