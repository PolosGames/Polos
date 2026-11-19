///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PASSES_DEBUG_GEOMETRY_HPP
#define POLOS_RENDERING_PASSES_DEBUG_GEOMETRY_HPP

#include "polos/rendering/vertex/debug_vertex.hpp"

#include <cstdint>
#include <vector>

namespace polos::rendering::detail
{

struct DebugGeometry
{
    std::vector<DebugVertex>   vertices;
    std::vector<std::uint16_t> indices;
};

auto CreateDebugSphereGeometry() -> DebugGeometry;
auto CreateDebugCircleGeometry() -> DebugGeometry;
auto CreateDebugCubeGeometry() -> DebugGeometry;

}// namespace polos::rendering::detail

#endif// POLOS_RENDERING_PASSES_DEBUG_GEOMETRY_HPP
