#version 460 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec2 attr_texcoord;

out vec2 frag_texcoord;

uniform mat4 model_matrix;

void main() {
    gl_Position = model_matrix * vec4(attr_position, 1.0);

    frag_texcoord = attr_texcoord;
}
