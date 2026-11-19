#version 450

// Bindings
layout(std140, set = 0, binding = 0) uniform CameraUniform
{
    mat4 viewProj;
    vec4 frustumPlanes[6];
}
camera;

struct DebugInstance
{
    mat4 transform;
    vec4 color;
};

layout(std430, set = 0, binding = 1) readonly buffer Instances
{
    DebugInstance instances[];
};

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iColor;

layout(location = 0) out vec4 oColor;

void main()
{
    DebugInstance instance = instances[gl_InstanceIndex];
    gl_Position = camera.viewProj * instance.transform * vec4(iPos, 1.0);
    oColor      = instance.color * vec4(iColor, 1.0);
}
