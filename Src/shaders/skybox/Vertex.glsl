#version 460 core

layout (location = 0) in vec3 attr_position;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 frag_texcoord;

void main() {
    frag_texcoord = attr_position;
    gl_Position = (projection_matrix * view_matrix * vec4(attr_position, 1.0)).xyww;
}
