#version 460 core

layout (location = 0) in vec2 i_UVCoords;

layout (location = 0) out vec4 o_FragColor;

uniform sampler2D u_Texture;
uniform mat4      u_TextureMatrix;

void main()
{
    //vec2 transformed_coords = (u_TextureMatrix * vec4(i_UVCoords, 0.0, 1.0)).xy;
    o_FragColor = texture(u_Texture, i_UVCoords);
//    o_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}