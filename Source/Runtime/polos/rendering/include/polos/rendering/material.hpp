///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_MATERIAL_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_MATERIAL_HPP

#include "polos/rendering/shader.hpp"

#include <cstdint>
#include <string>

namespace polos::rendering
{

enum class MaterialType : std::uint8_t
{
    kOpaque,
    kTransparent,
};

struct texture_2d;

struct material
{
    std::string name;

    shader                      mat_shader;
    std::shared_ptr<texture_2d> albedo_texture;

    MaterialType mat_type;

    VkPipeline       pipeline;
    VkPipelineLayout pipeline_layout;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_MATERIAL_HPP
