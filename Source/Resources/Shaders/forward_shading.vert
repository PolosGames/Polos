#version 450

struct VertexOutput
{
    vec4 color;
    vec2 texCoord;
};

struct MeshInstance
{
    vec3  position;
    uint  parentId;
    vec4  rotation;
    float scale;
    uint  meshId;
    uint  textureIdx;
};

layout(std430, set = 1, binding = 0) readonly buffer MeshInstances
{
    MeshInstance instances[];
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
    vec4 frustumPlanes[6];
}
ubo;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec4 iTangent;
layout(location = 3) in vec2 iTexCoord;
layout(location = 4) in vec4 iColor;
layout(location = 0) out VertexOutput oVertOut;
layout(location = 2) out flat uint oTexIndex;

void main()
{
    uint     entityIdx   = visibleMatrixIndices[gl_InstanceIndex];
    mat4     worldMatrix = worldMatrices[entityIdx];
    MeshInstance inst        = instances[entityIdx];

    gl_Position = ubo.viewProj * worldMatrix * vec4(iPosition, 1.0);

    oVertOut.color    = iColor;
    oVertOut.texCoord = iTexCoord;
    oTexIndex         = inst.textureIdx;
}