#version 450

layout(binding = 1) uniform sampler2D u_texture;

layout(location = 0) in vec3 i_frag_color;
layout(location = 1) in vec2 i_frag_tex_coord;

layout(location = 0) out vec4 o_target_color;

void main()
{
    o_target_color = vec4(i_frag_color * texture(u_texture, i_frag_tex_coord).rgb, 1.0);
}
