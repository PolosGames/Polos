///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VERTEX_VERTEX_INPUT_DESCRIPTION_HPP
#define POLOS_RENDERING_VERTEX_VERTEX_INPUT_DESCRIPTION_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::rendering
{
/// @brief A struct to hold the generated Vulkan descriptions.
struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription>   bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_VERTEX_VERTEX_INPUT_DESCRIPTION_HPP
