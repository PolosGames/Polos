///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SSBO_QUAD_INSTANCE_HPP
#define POLOS_RENDERING_SSBO_QUAD_INSTANCE_HPP

#include "polos/rendering/resources/buffer.hpp"

#include <glm/glm.hpp>

#include <cstddef>
#include <memory>

namespace polos::rendering
{

struct QuadInstance
{
    glm::mat4     model{1.0F};
    glm::vec4     color{1.0F, 1.0F, 1.0F, 1.0F};
    glm::vec2     texCoordScissor{0.0F, 0.0F};
    std::uint32_t textureIndex{0};
    float         tilingFactor{1.0F};
};

auto CreateQuadInstanceStorageBuffer(std::size_t tInstanceCount) -> std::unique_ptr<Buffer>;

}// namespace polos::rendering

#endif// POLOS_RENDERING_SSBO_QUAD_INSTANCE_HPP
