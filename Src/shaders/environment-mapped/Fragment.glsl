#version 460 core

in Data {
    vec3 frag_position;
    vec4 frag_position_light_space;
    vec3 frag_normal;
    vec2 frag_texcoord;
} frag_in;

out vec4 frag_color;

uniform vec3 view_position;
uniform samplerCube skybox_map;

void main() {
    vec3 N = normalize(frag_in.frag_normal);
    vec3 I = normalize(frag_in.frag_position - view_position);
    vec3 R = reflect(I, N);
    frag_color = vec4(mix(texture(skybox_map, R).rgb, vec3(1.0, 0.0, 0.0), 0.5), 1.0);
}
