#version 450

// Set 0: per-view
layout(std140, set = 0, binding = 0) uniform PerView
{
    mat4 view_projection;
    vec3 eye_position;
}
u_view;

// Set 1: per-model
layout(std140, set = 1, binding = 0) uniform PerModel
{
    mat4 model_transform;
    mat4 inverse_transpose_model_transform;
}
u_model;

// Locations follow polos::Vertex; slot 2 is the unused vertex color.
layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 3) in vec2 i_tex_coord;

layout(location = 0) out vec3 o_world_position;
layout(location = 1) out vec3 o_world_normal;
layout(location = 2) out vec2 o_tex_coord;

void main()
{
    vec3 world_position = (u_model.model_transform * vec4(i_position, 1.0)).xyz;
    vec3 world_normal   = (u_model.inverse_transpose_model_transform * vec4(i_normal, 0.0)).xyz;
    o_world_position    = world_position;
    o_world_normal      = normalize(world_normal);
    o_tex_coord         = i_tex_coord;
    gl_Position         = u_view.view_projection * vec4(world_position, 1.0);
}
