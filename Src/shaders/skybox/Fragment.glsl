#version 460 core

in vec3 frag_texcoord;

out vec4 frag_color;

uniform samplerCube skybox_map;

void main() {
    frag_color = texture(skybox_map, frag_texcoord);
}
