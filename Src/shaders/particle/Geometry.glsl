#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec3 camera_right;
uniform vec3 camera_up;

out Data {
    vec2 frag_texcoord;
} geom_out;

void main() {
    mat4 VP = projection_matrix * view_matrix;
    vec3 position = gl_in[0].gl_Position.xyz;
    float size = gl_in[0].gl_Position.w;

    vec4 v0 = VP * vec4(position + camera_right * 0.5 * size + camera_up * -0.5 * size, 1.0);
    geom_out.frag_texcoord = vec2(0.0, 0.0);
    gl_Position = v0;
    EmitVertex();

    vec4 v1 = VP * vec4(position + camera_right * 0.5 * size + camera_up * 0.5 * size, 1.0);
    geom_out.frag_texcoord = vec2(0.0, 1.0);
    gl_Position = v1;
    EmitVertex();

    vec4 v2 = VP * vec4(position + camera_right * -0.5 * size + camera_up * -0.5 * size, 1.0);
    geom_out.frag_texcoord = vec2(1.0, 0.0);
    gl_Position = v2;
    EmitVertex();

    vec4 v3 = VP * vec4(position + camera_right * -0.5 * size + camera_up * 0.5 * size, 1.0);
    geom_out.frag_texcoord = vec2(1.0, 1.0);
    gl_Position = v3;
    EmitVertex();

    EndPrimitive();
}
