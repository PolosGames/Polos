#version 450

const int k_max_point_lights = 16;
const int k_max_dir_lights   = 4;

// Set 0: per-view
layout(std140, set = 0, binding = 0) uniform PerView
{
    mat4 view_projection;
    vec3 eye_position;
}
u_view;

// Set 2: material
layout(std140, set = 2, binding = 0) uniform Material
{
    vec3  diffuse_color;
    float _pad0;
    vec3  specular_color;
    float specularity;
}
u_material;

// Explicit padding keeps the vec3 members on the std140 16-byte stride the host writes.
struct PointLight
{
    vec3  position;
    float _pad0;
    vec3  intensity;
    float _pad1;
};

struct DirectionalLight
{
    vec3  direction;
    float _pad0;
    vec3  intensity;
    float _pad1;
};

// Set 3: lighting environment
layout(std140, set = 3, binding = 0) uniform LightingEnv
{
    int              num_point_lights;
    int              num_dir_lights;
    vec2             _pad;
    PointLight       point_lights[k_max_point_lights];
    DirectionalLight dir_lights[k_max_dir_lights];
}
u_lights;

layout(location = 0) in vec3 i_world_position;
layout(location = 1) in vec3 i_world_normal;
layout(location = 2) in vec2 i_tex_coord;

layout(location = 0) out vec4 o_target_color;

vec3 BlinnPhong(vec3 t_view, vec3 t_light, vec3 t_normal, vec3 t_kd, vec3 t_ks, float t_specularity)
{
    float n_dot_l = clamp(dot(t_normal, t_light), 0.0, 1.0);
    vec3  half_v  = normalize(t_light + t_view);
    float n_dot_h = clamp(dot(t_normal, half_v), 0.0, 1.0);
    return t_kd * n_dot_l + t_ks * pow(n_dot_h, t_specularity);
}

vec3 EvaluatePointLight(PointLight t_light,
                        vec3       t_world_position,
                        vec3       t_normal,
                        vec3       t_view,
                        vec3       t_kd,
                        vec3       t_ks,
                        float      t_specularity)
{
    vec3  delta       = t_light.position - t_world_position;
    float distance    = length(delta);
    vec3  light_dir   = normalize(delta);
    vec3  illuminance = t_light.intensity / (distance * distance);
    return illuminance * BlinnPhong(t_view, light_dir, t_normal, t_kd, t_ks, t_specularity);
}

vec3 EvaluateDirectionalLight(DirectionalLight t_light,
                              vec3             t_normal,
                              vec3             t_view,
                              vec3             t_kd,
                              vec3             t_ks,
                              float            t_specularity)
{
    return t_light.intensity * BlinnPhong(t_view, t_light.direction, t_normal, t_kd, t_ks, t_specularity);
}

void main()
{
    vec3  normal      = normalize(i_world_normal);
    vec3  view        = normalize(u_view.eye_position - i_world_position);
    vec3  kd          = u_material.diffuse_color;
    vec3  ks          = u_material.specular_color;
    float specularity = u_material.specularity;
    vec3  color       = vec3(0.0);
    for (int i = 0; i < u_lights.num_point_lights; ++i)
    {
        color += EvaluatePointLight(u_lights.point_lights[i], i_world_position, normal, view, kd, ks, specularity);
    }
    for (int j = 0; j < u_lights.num_dir_lights; ++j)
    {
        color += EvaluateDirectionalLight(u_lights.dir_lights[j], normal, view, kd, ks, specularity);
    }
    color += kd * 0.1;// ambient
    o_target_color = vec4(clamp(color, 0.0, 1.0), 1.0);
}
