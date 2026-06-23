///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SHADER_HPP
#define POLOS_RENDERING_SHADER_HPP

#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <string>

namespace polos::rendering
{

enum class ShaderStage : std::uint8_t
{
    kVertex      = VK_SHADER_STAGE_VERTEX_BIT,
    kFragment    = VK_SHADER_STAGE_FRAGMENT_BIT,
    kCompute     = VK_SHADER_STAGE_COMPUTE_BIT,
    kGeometry    = VK_SHADER_STAGE_GEOMETRY_BIT,
    kTessControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    kTessEval    = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
};

struct Shader
{
    utils::string_id name;
    ShaderStage      stage;
    VkShaderModule   module;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SHADER_HPP
