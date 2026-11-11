#version 450

struct VertexOutput
{
    vec4 color;
};

layout(std430, set = 2, binding = 0) readonly buffer VisibleMatrixIndices
{
    uint visibleMatrixIndices[];
};
layout(std430, set = 2, binding = 1) readonly buffer OutputMatrices
{
    mat4 worldMatrices[];
};

layout(std140, set = 0, binding = 0) uniform CameraUniform
{
    mat4 viewProj;
}
ubo;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec4 iTangent;
layout(location = 3) in vec2 iTexCoord;
layout(location = 4) in vec4 iColor;
layout(location = 0) out VertexOutput oVertOut;

void main()
{
    mat4 worldMatrix = worldMatrices[visibleMatrixIndices[gl_InstanceIndex]];

    gl_Position = ubo.viewProj * worldMatrix * vec4(iPosition, 1.0);

    oVertOut.color = iColor;
}