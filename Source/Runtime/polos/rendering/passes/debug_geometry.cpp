///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/debug_geometry.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace polos::rendering::detail
{
namespace
{

constexpr std::uint16_t kCircleSegments{32U};

void AppendCircle(DebugGeometry& tGeometry, glm::vec3 tAxisU, glm::vec3 tAxisV)
{
    auto const base = static_cast<std::uint16_t>(tGeometry.vertices.size());
    for (std::uint16_t i{0U}; i < kCircleSegments; ++i)
    {
        float const angle = glm::two_pi<float>() * static_cast<float>(i) / static_cast<float>(kCircleSegments);
        tGeometry.vertices.push_back({.position = glm::cos(angle) * tAxisU + glm::sin(angle) * tAxisV});
        tGeometry.indices.push_back(static_cast<std::uint16_t>(base + i));
        tGeometry.indices.push_back(static_cast<std::uint16_t>(base + ((i + 1U) % kCircleSegments)));
    }
}

}// namespace

auto CreateDebugSphereGeometry() -> DebugGeometry
{
    DebugGeometry geometry;
    geometry.vertices.reserve(3U * kCircleSegments);
    geometry.indices.reserve(6U * kCircleSegments);

    // Three closed great circles in the XY, XZ, and YZ planes.
    AppendCircle(geometry, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F});
    AppendCircle(geometry, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F});
    AppendCircle(geometry, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 1.0F});
    return geometry;
}

auto CreateDebugCircleGeometry() -> DebugGeometry
{
    DebugGeometry geometry;
    geometry.vertices.reserve(kCircleSegments);
    geometry.indices.reserve(2U * kCircleSegments);
    AppendCircle(geometry, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F});
    return geometry;
}

auto CreateDebugCubeGeometry() -> DebugGeometry
{
    return {
        .vertices =
            {
                {.position = {-0.5F, -0.5F, 0.5F}},
                {.position = {0.5F, -0.5F, 0.5F}},
                {.position = {0.5F, 0.5F, 0.5F}},
                {.position = {-0.5F, 0.5F, 0.5F}},
                {.position = {-0.5F, -0.5F, -0.5F}},
                {.position = {0.5F, -0.5F, -0.5F}},
                {.position = {0.5F, 0.5F, -0.5F}},
                {.position = {-0.5F, 0.5F, -0.5F}},
            },
        .indices = {
            0U,
            1U,
            1U,
            2U,
            2U,
            3U,
            3U,
            0U,
            4U,
            5U,
            5U,
            6U,
            6U,
            7U,
            7U,
            4U,
            0U,
            4U,
            1U,
            5U,
            2U,
            6U,
            3U,
            7U,
        },
    };
}

}// namespace polos::rendering::detail
