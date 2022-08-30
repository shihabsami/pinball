#version 460 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_texcoord;

out Data {
    vec3 geom_normal;
    vec2 geom_texcoord;
} vert_out;

void main() {
    gl_Position = vec4(attr_position, 1.0);

    vert_out.geom_normal = attr_normal;
    vert_out.geom_texcoord = attr_texcoord;
}
