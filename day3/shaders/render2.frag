#version 330

in vec2 texcoord;

out vec4 out_color;

uniform sampler2D u_texture;

void main() {
   
    out_color = vec4(texture(u_texture, texcoord).rgb, 1.0);

}

