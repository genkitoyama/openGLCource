#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;

out vec2 texcoord;

uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    texcoord = in_texcoord;
}
