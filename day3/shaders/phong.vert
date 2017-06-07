#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;

uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    
    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = normalize((u_normMat * vec4(in_normal, 0.0)).xyz);
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
}


//void main(void)
//{
//    // 頂点位置，法線ベクトル，光線ベクトル，視線ベクトル，中間ベクトル
//    vec4 position = u_mvMat * vec4(in_position, 1.0);
//    vec3 normal = normalize(u_normMat * vec4(in_normal, 1.0));
//    vec3 light = normalize((vec4(u_lightPos, 1.0) * position.w - u_lightPos.x * position).xyz);
//    vec3 view = -normalize(position.xyz);
//    vec3 halfway = normalize(light + view);
//
//    // 拡散反射率と鏡面反射率
//    float diffuse = max(dot(light, normal), 0.0);
//    float specular = pow(max(dot(normal, halfway), 0.0), gl_FrontMaterial.shininess);
//
//    // 頂点の色
//    gl_FrontColor = gl_FrontLightProduct[0].ambient
//    + gl_FrontLightProduct[0].diffuse * diffuse
//    + gl_FrontLightProduct[0].specular * specular;
//
//    // 頂点位置
//    gl_Position = ftransform();
//}
