#version 450

struct VertexOutput
{
    vec4 color;
};

layout(location = 0) in VertexOutput iVertOut;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = iVertOut.color;
}