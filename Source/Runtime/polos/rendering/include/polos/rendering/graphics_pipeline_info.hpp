//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/vertex.hpp"

namespace polos::rendering
{

struct graphics_pipeline_info
{
    VkShaderModule vertex_shader;
    VkShaderModule fragment_shader;

    VkPrimitiveTopology    topology;
    VertexInputDescription vertex_input;

    VkPolygonMode   polygon_mode;
    VkCullModeFlags cull_mode;
    VkFrontFace     front_face;
    bool            depth_bias_enable;

    VkSampleCountFlagBits multisampling;

    bool        depth_test_enable;
    bool        depth_write_enable;
    VkCompareOp depth_compare_op;

    // For a shadow pass, this will be empty as we don't write to color targets.
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;

    VkRenderPass  render_pass;
    std::uint32_t subpass;

    std::vector<VkDynamicState> dynamic_states;
};

}// namespace polos::rendering

namespace std
{

template<>
struct hash<polos::rendering::graphics_pipeline_info>
{
    using argument_type = polos::rendering::graphics_pipeline_info;
    using result_type   = std::size_t;
    [[nodiscard]] static result_type operator()(polos::rendering::graphics_pipeline_info const& t_description) noexcept
    {
        // A robust way to combine multiple hash values.
        size_t seed         = 0;
        auto   hash_combine = [&](size_t& seed, auto const& value) {
            std::hash<std::decay_t<decltype(value)>> hasher;
            seed ^= hasher(value) + 0x9e3779b97f4a7c15 + (seed << 6) + (seed >> 2);
        };

        hash_combine(seed, reinterpret_cast<uintptr_t>(t_description.vertex_shader));
        hash_combine(seed, reinterpret_cast<uintptr_t>(t_description.fragment_shader));

        hash_combine(seed, static_cast<int>(t_description.topology));
        hash_combine(seed, static_cast<int>(t_description.polygon_mode));
        hash_combine(seed, static_cast<int>(t_description.cull_mode));
        hash_combine(seed, static_cast<int>(t_description.front_face));
        hash_combine(seed, static_cast<int>(t_description.multisampling));
        hash_combine(seed, static_cast<int>(t_description.depth_compare_op));

        hash_combine(seed, t_description.depth_test_enable);
        hash_combine(seed, t_description.depth_write_enable);
        hash_combine(seed, t_description.depth_bias_enable);

        hash_combine(seed, reinterpret_cast<uintptr_t>(t_description.render_pass));
        hash_combine(seed, t_description.subpass);


        hash_combine(seed, t_description.vertex_input.bindings.size());
        hash_combine(seed, t_description.vertex_input.attributes.size());
        for (const auto& binding : t_description.vertex_input.bindings)
        {
            hash_combine(seed, binding.binding);
            hash_combine(seed, binding.stride);
            hash_combine(seed, static_cast<int>(binding.inputRate));
        }
        for (const auto& attribute : t_description.vertex_input.attributes)
        {
            hash_combine(seed, attribute.location);
            hash_combine(seed, attribute.binding);
            hash_combine(seed, static_cast<int>(attribute.format));
            hash_combine(seed, attribute.offset);
        }

        // 6. Hash vectors of states
        for (const auto& blendAttachment : t_description.color_blend_attachments)
        {
            hash_combine(seed, blendAttachment.blendEnable);
            // ... hash all other members of the blendAttachment struct
        }
        for (const auto& dynamicState : t_description.dynamic_states)
        {
            hash_combine(seed, static_cast<int>(dynamicState));
        }
        return seed;
    }
};

}// namespace std

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP_
