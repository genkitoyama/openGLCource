#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_tangentCameraSpace;
in vec3 f_binormalCameraSpace;
in vec3 f_lightPosCameraSpace;
in vec2 f_texcoord;

out vec4 out_color;

uniform sampler2D u_diffTexture;
uniform sampler2D u_bumpTexture;

void main() {
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 T = normalize(f_tangentCameraSpace);
    vec3 B = normalize(f_binormalCameraSpace);
    
//    vec2 texcoord = f_texcoord * 4.0;

    vec3 localN = texture(u_bumpTexture, f_texcoord).xyz * 2.0 - 1.0;
    vec3 bumpN = normalize(N * localN.z + T * localN.x + B * localN.y);
    
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);
    
    float ndotl = max(0.0, dot(bumpN, L));
    vec3 diffuse = texture(u_diffTexture, f_texcoord).xyz * ndotl;
    
    out_color = vec4(diffuse, 1.0);
}
