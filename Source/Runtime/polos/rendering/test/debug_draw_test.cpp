#include "polos/rendering/debug_draw.hpp"
#include "polos/rendering/passes/debug_geometry.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <set>
#include <utility>
#include <vector>

namespace polos::rendering
{
namespace
{

auto CheckLineGeometry(detail::DebugGeometry const& tGeometry) -> std::vector<std::vector<std::size_t>>
{
    EXPECT_FALSE(tGeometry.vertices.empty());
    EXPECT_EQ(tGeometry.indices.size() % 2U, 0U);
    std::vector<std::vector<std::size_t>>             neighbours(tGeometry.vertices.size());
    std::set<std::pair<std::uint16_t, std::uint16_t>> edges;
    for (std::size_t i{0U}; i + 1U < tGeometry.indices.size(); i += 2U)
    {
        auto const a = tGeometry.indices[i];
        auto const b = tGeometry.indices[i + 1U];
        EXPECT_LT(a, tGeometry.vertices.size());
        EXPECT_LT(b, tGeometry.vertices.size());
        if (a >= tGeometry.vertices.size() || b >= tGeometry.vertices.size())
        {
            continue;
        }
        EXPECT_GT(glm::distance(tGeometry.vertices[a].position, tGeometry.vertices[b].position), 0.001F);
        EXPECT_TRUE(edges.emplace(std::min(a, b), std::max(a, b)).second);
        neighbours[a].push_back(b);
        neighbours[b].push_back(a);
    }
    return neighbours;
}

auto ConnectedComponents(std::vector<std::vector<std::size_t>> const& tNeighbours)
    -> std::vector<std::vector<std::size_t>>
{
    std::vector<std::vector<std::size_t>> components;
    std::vector<bool>                     visited(tNeighbours.size());
    for (std::size_t i{0U}; i < visited.size(); ++i)
    {
        if (visited[i])
        {
            continue;
        }
        auto&                    component = components.emplace_back();
        std::vector<std::size_t> pending{i};
        visited[i] = true;
        while (!pending.empty())
        {
            auto const vertex = pending.back();
            pending.pop_back();
            component.push_back(vertex);
            for (auto const neighbour : tNeighbours[vertex])
            {
                if (!visited[neighbour])
                {
                    visited[neighbour] = true;
                    pending.push_back(neighbour);
                }
            }
        }
    }
    return components;
}

TEST(DebugGeometryTest, CubeHasExactlyTwelveEdgesWithoutFaceDiagonals)
{
    auto const geometry = detail::CreateDebugCubeGeometry();
    ASSERT_EQ(geometry.vertices.size(), 8U);
    ASSERT_EQ(geometry.indices.size(), 24U);
    auto const neighbours = CheckLineGeometry(geometry);
    EXPECT_EQ(ConnectedComponents(neighbours).size(), 1U);

    for (std::size_t i{0U}; i < geometry.vertices.size(); ++i)
    {
        auto const position = geometry.vertices[i].position;
        EXPECT_FLOAT_EQ(glm::abs(position.x), 0.5F);
        EXPECT_FLOAT_EQ(glm::abs(position.y), 0.5F);
        EXPECT_FLOAT_EQ(glm::abs(position.z), 0.5F);
        EXPECT_EQ(neighbours[i].size(), 3U);
        for (auto const neighbour : neighbours[i])
        {
            EXPECT_FLOAT_EQ(glm::distance(position, geometry.vertices[neighbour].position), 1.0F);
        }
    }
}

TEST(DebugGeometryTest, CircleIsOneClosedUnitRingInTheXyPlane)
{
    auto const geometry   = detail::CreateDebugCircleGeometry();
    auto const neighbours = CheckLineGeometry(geometry);
    EXPECT_EQ(ConnectedComponents(neighbours).size(), 1U);
    for (std::size_t i{0U}; i < geometry.vertices.size(); ++i)
    {
        EXPECT_EQ(neighbours[i].size(), 2U);
        EXPECT_NEAR(glm::length(geometry.vertices[i].position), 1.0F, 0.00001F);
        EXPECT_FLOAT_EQ(geometry.vertices[i].position.z, 0.0F);
    }
}

TEST(DebugGeometryTest, SphereContainsThreeClosedOrthogonalUnitRings)
{
    auto const geometry   = detail::CreateDebugSphereGeometry();
    auto const neighbours = CheckLineGeometry(geometry);
    auto const components = ConnectedComponents(neighbours);
    ASSERT_EQ(components.size(), 3U);
    for (std::size_t i{0U}; i < geometry.vertices.size(); ++i)
    {
        EXPECT_EQ(neighbours[i].size(), 2U);
        EXPECT_NEAR(glm::length(geometry.vertices[i].position), 1.0F, 0.00001F);
    }

    std::array<bool, 3U> planes{};
    for (auto const& component : components)
    {
        EXPECT_GE(component.size(), 8U);
        std::size_t planarAxes{0U};
        for (glm::length_t axis{0}; axis < 3; ++axis)
        {
            bool const inPlane = std::ranges::all_of(component, [&](auto tVertex) {
                return glm::abs(geometry.vertices[tVertex].position[axis]) < 0.00001F;
            });
            if (inPlane)
            {
                EXPECT_FALSE(planes[static_cast<std::size_t>(axis)]);
                planes[static_cast<std::size_t>(axis)] = true;
                ++planarAxes;
            }
        }
        EXPECT_EQ(planarAxes, 1U);
    }
    EXPECT_TRUE(std::ranges::all_of(planes, [](bool tPlane) {
        return tPlane;
    }));
}

TEST(DebugDrawQueueTest, ClearingAFrameRemovesEveryPrimitiveAndRetainsStorage)
{
    DebugDrawQueue                                queue;
    DebugInstance const                           instance{.color = {1.0F, 0.0F, 0.0F, 1.0F}};
    std::array<std::size_t, kDebugPrimitiveCount> capacities{};
    for (std::size_t i{0U}; i < kDebugPrimitiveCount; ++i)
    {
        queue.Push(static_cast<DebugPrimitive>(i), instance);
        ASSERT_EQ(queue.batches[i].size(), 1U);
        EXPECT_EQ(queue.batches[i].front().color, instance.color);
        capacities[i] = queue.batches[i].capacity();
    }
    queue.Clear();
    for (std::size_t i{0U}; i < kDebugPrimitiveCount; ++i)
    {
        EXPECT_TRUE(queue.batches[i].empty());
        EXPECT_EQ(queue.batches[i].capacity(), capacities[i]);
    }
}

}// namespace
}// namespace polos::rendering
