#version 460 core

layout (location = 0) in vec2 i_UVCoords;

layout (location = 0) out vec4 o_FragColor;

uniform sampler2D u_Texture;

void main()
{
    o_FragColor = texture(u_Texture, i_UVCoords);
//    o_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}