#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;

out vec2 texcoord;

uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    
    texcoord = in_texcoord;
}
