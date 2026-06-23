///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_MATERIAL_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_MATERIAL_HPP

#include "polos/rendering/shader.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace polos::rendering
{

enum class MaterialType : std::uint8_t
{
    kOpaque,
    kTransparent,
};

struct AllocatedImage;

struct alignas(128) Material
{
    std::string name;

    Shader                           mat_shader;
    std::shared_ptr<AllocatedImage> albedo_texture;

    MaterialType mat_type;

    VkPipeline       pipeline;
    VkPipelineLayout pipeline_layout;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_MATERIAL_HPP
