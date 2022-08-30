#version 460 core

layout (location = 0) in vec3 attr_position;

uniform mat4 model_matrix;
uniform mat4 light_space_matrix;

void main() {
    gl_Position = light_space_matrix * model_matrix * vec4(attr_position, 1.0);
}
