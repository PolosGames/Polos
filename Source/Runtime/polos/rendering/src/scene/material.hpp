///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_SCENE_MATERIAL_HPP
#define POLOS_RENDERING_SRC_SCENE_MATERIAL_HPP

#include "resources/shader.hpp"

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

struct Image;

struct alignas(128) Material
{
    std::string name;

    Shader                 matShader;
    std::shared_ptr<Image> albedoTexture;

    MaterialType matType;

    VkPipeline       pipeline;
    VkPipelineLayout pipelineLayout;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_SCENE_MATERIAL_HPP
