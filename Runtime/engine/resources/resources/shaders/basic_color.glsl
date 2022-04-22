#shader vertex
#version 460 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Color;

out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragColor = a_Color;
    gl_Position = projection * view * model * vec4(a_Pos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 fragColor;

void main() {
    FragColor = vec4(fragColor, 1.0);
}
