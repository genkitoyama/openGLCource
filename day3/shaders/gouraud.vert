#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

vec3 f_positionCameraSpace;
vec3 f_normalCameraSpace;
vec3 f_lightPosCameraSpace;

uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;

out vec4 f_color;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    
    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
    
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);
    
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));
    vec3 diffuse = u_diffColor * ndotl;
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambiColor;
    
    f_color = vec4(diffuse + specular + ambient, 1.0);
}
