#version 450

layout(location = 0) in vec3 i_frag_color;

layout(location = 0) out vec4 o_target_color;

void main()
{
    o_target_color = vec4(i_frag_color, 1.0);
}
