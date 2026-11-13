#version 450

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_terminate_invocation : require

struct VertexOutput
{
    vec4 color;
    vec2 texCoord;
};

struct LightInstance
{
    vec3  position;
    float ambientFactor;
    vec3  color;
    float specularFactor;
};

layout(std430, set = 0, binding = 1) readonly buffer Lights
{
    LightInstance lights[];
};
layout(set = 3, binding = 0) uniform sampler2D uTextures[];


layout(push_constant) uniform SceneLightInfo
{
    vec3  color;
    float ambientFactor;
    uint  lightCount;
}
sceneLightInfo;

layout(location = 0) in VertexOutput iVertOut;
layout(location = 2) in flat uint iTexIndex;
layout(location = 3) in vec3 iNormal;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = iVertOut.color;

    vec4 color = iVertOut.color * texture(uTextures[nonuniformEXT(iTexIndex)], iVertOut.texCoord);

    if (color.a == 0.0)
    {
        terminateInvocation;
    }

    vec3 ambient = sceneLightInfo.ambientFactor * sceneLightInfo.color;
    vec3 norm    = normalize(iNormal);
    // vec3 lightDir = normalize()

    oColor = vec4(ambient, 1.0) * color;
}
