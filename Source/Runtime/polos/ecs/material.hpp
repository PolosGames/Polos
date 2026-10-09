///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_MATERIAL_HPP
#define POLOS_ECS_MATERIAL_HPP

#include "polos/rendering/resources/shader.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace polos::rendering
{
class Image;
}// namespace polos::rendering

namespace polos::ecs
{

enum class MaterialType : std::uint8_t
{
    kOpaque,
    kTransparent,
};

struct alignas(128) Material
{
    std::string name;

    rendering::Shader                matShader;
    std::shared_ptr<rendering::Image> albedoTexture;

    MaterialType matType;

    VkPipeline       pipeline;
    VkPipelineLayout pipelineLayout;
};

}// namespace polos::ecs

#endif// POLOS_ECS_MATERIAL_HPP
