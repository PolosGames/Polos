#version 450

struct QuadInstance
{
    mat4  model;
    vec4  color;
    vec2  texCoordScissor;
    uint  textureIndex;
    float tilingFactor;
};

layout(std430, set = 0, binding = 2) readonly buffer InstanceBuffer
{
    QuadInstance instances[];
};

layout(std140, set = 0, binding = 0) uniform CameraUniform
{
    mat4 viewProj;
}
ubo;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTexCoord;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec2 oTexCoord;
layout(location = 2) out flat uint oTexIndex;
layout(location = 3) out float oTilingFactor;

void main()
{
    QuadInstance inst = instances[gl_InstanceIndex];

    gl_Position   = ubo.viewProj * inst.model * vec4(iPosition, 1.0);
    oColor        = inst.color;
    oTexCoord     = iTexCoord;
    oTexIndex     = inst.textureIndex;
    oTilingFactor = inst.tilingFactor;
}