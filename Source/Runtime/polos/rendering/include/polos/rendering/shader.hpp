///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SHADER_HPP
#define POLOS_RENDERING_SHADER_HPP

#include <vulkan/vulkan.h>

#include <string>

namespace polos::rendering
{

/// @brief Placeholder for using VkShaderModule
struct shader
{
    std::string    name;
    VkShaderModule module;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SHADER_HPP
