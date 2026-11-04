#include "polos/rendering/vertex/primitive_vertex.hpp"
#include "polos/rendering/vertex/quad_vertex.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <span>

namespace polos::rendering
{
namespace
{
struct ExpectedAttribute
{
    std::uint32_t location;
    VkFormat      format;
    std::size_t   offset;
};

auto ExpectVertexLayout(
    VertexInputDescription const&      tLayout,
    std::size_t                        tStride,
    std::span<ExpectedAttribute const> tAttributes) -> void
{
    ASSERT_EQ(tLayout.bindings.size(), 1U);
    EXPECT_EQ(tLayout.bindings[0].binding, 0U);
    EXPECT_EQ(tLayout.bindings[0].stride, tStride);
    EXPECT_EQ(tLayout.bindings[0].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
    ASSERT_EQ(tLayout.attributes.size(), tAttributes.size());

    for (std::size_t i{0U}; i < tAttributes.size(); ++i)
    {
        SCOPED_TRACE(i);
        auto const& actual   = tLayout.attributes[i];
        auto const& expected = tAttributes[i];
        EXPECT_EQ(actual.binding, 0U);
        EXPECT_EQ(actual.location, expected.location);
        EXPECT_EQ(actual.format, expected.format);
        EXPECT_EQ(actual.offset, expected.offset);
        EXPECT_LT(actual.offset, tStride);
    }
}

TEST(VertexLayoutTest, QuadInputsMatchShaderLocationsAndCpuMemberOffsets)
{
    std::array const expected{
        ExpectedAttribute{0U, VK_FORMAT_R32G32B32_SFLOAT, offsetof(QuadVertex, position)},
        ExpectedAttribute{1U, VK_FORMAT_R32G32_SFLOAT, offsetof(QuadVertex, texCoord)},
    };

    auto const layout = QuadVertex::CreateVertexInputDescription();

    ExpectVertexLayout(layout, sizeof(QuadVertex), expected);
}

TEST(VertexLayoutTest, PrimitiveInputsUseExpectedFormatsAndCpuMemberOffsets)
{
    std::array const expected{
        ExpectedAttribute{0U, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PrimitiveVertex, position)},
        ExpectedAttribute{1U, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PrimitiveVertex, normal)},
        ExpectedAttribute{2U, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PrimitiveVertex, tangent)},
        ExpectedAttribute{3U, VK_FORMAT_R32G32_SFLOAT, offsetof(PrimitiveVertex, texCoord)},
        ExpectedAttribute{4U, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PrimitiveVertex, color)},
    };

    auto const layout = PrimitiveVertex::CreateVertexInputDescription();

    ExpectVertexLayout(layout, sizeof(PrimitiveVertex), expected);
}

}// namespace
}// namespace polos::rendering
