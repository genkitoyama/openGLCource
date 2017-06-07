#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

out vec4 out_color;

uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;

float roughness = 0.5;

void main() {
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);
    
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));
    
    float angleHN = acos(ndoth);
    float angleLN = acos(ndotl);
    
    float alpha = max(angleHN, angleLN);
    float beta  = min(angleHN, angleLN);
    
    float gamma = dot(V - N * dot(V, N), L - N * dot(L, N));
    
    float A = 1.0 - 0.5 * (roughness*roughness/(roughness - 2 + 0.33));
    float B = 0.45 * (roughness*roughness/(roughness*roughness + 0.09));
    
    vec3 diffuse = u_diffColor * ndotl * (A+B*max(0, gamma)*sin(alpha)*tan(beta));
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambiColor;
    
    out_color = vec4(diffuse + ambient, 1.0);
}
