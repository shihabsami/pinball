#version 460 core

in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D depth_map_dl;

void main() {
    float depth = texture(depth_map_dl, frag_texcoord).r;
    frag_color = vec4(vec3(depth), 1.0);
}
