#version 460 core

layout (location = 0) in vec4 particle_attr;

void main() {
    gl_Position = particle_attr;
}
