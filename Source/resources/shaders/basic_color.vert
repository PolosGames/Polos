#version 460 core

layout (location = 0) in vec3 i_Pos;
layout (location = 1) in vec3 i_Normal;
layout (location = 2) in vec2 i_UVCoords;
layout (location = 3) in vec3 i_Color;

layout (location = 0) out vec3 o_FragColor;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vec3 l_OutColor = i_Color;
    
    // if the user gave the colors with the range of 0-255
    if(l_OutColor.x > 1.0 || l_OutColor.y > 1.0 || l_OutColor.z > 1.0)
    {
        l_OutColor /= 255.0;
    }

    o_FragColor = l_OutColor;
    gl_Position = u_Projection * u_View * u_Model * vec4(i_Pos, 1.0);
}