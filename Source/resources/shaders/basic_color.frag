#version 460 core

layout (location = 0) in vec3 i_FragColor;

layout (location = 0) out vec4 o_FinalColor;

void main() {
    o_FinalColor = vec4(i_FragColor, 1.0);
}