#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 projection;
}
ubo;

struct QuadInstance
{
    mat4 model;
    vec4 color;
};

layout(std430, set = 0, binding = 2) readonly buffer InstanceBuffer
{
    QuadInstance instances[];
};

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec3 i_color;
layout(location = 3) in vec2 i_tex_coord;

layout(location = 0) out vec3 o_frag_color;
layout(location = 1) out vec2 o_frag_tex_coord;

void main()
{
    QuadInstance inst = instances[gl_InstanceIndex];

    gl_Position      = ubo.projection * ubo.view * inst.model * vec4(i_position, 1.0);
    o_frag_color     = i_color * inst.color.rgb;
    o_frag_tex_coord = i_tex_coord;
}
