#version 450

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_terminate_invocation : require

struct VertexOutput
{
    vec4 color;
    vec2 texCoord;
};

layout(set = 3, binding = 0) uniform sampler2D uTextures[];

layout(location = 0) in VertexOutput iVertOut;
layout(location = 2) in flat uint iTexIndex;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = iVertOut.color;

    vec4 color = iVertOut.color * texture(uTextures[nonuniformEXT(iTexIndex)], iVertOut.texCoord);

    if (color.a == 0.0)
    {
        terminateInvocation;
    }

    oColor = color;
}
