#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 light_space_matrix;

in Data {
    vec3 geom_normal;
    vec2 geom_texcoord;
} geom_in[];

out Data {
    vec3 frag_position;
    vec4 frag_position_light_space;
    vec3 frag_normal;
    vec2 frag_texcoord;
} geom_out;

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
        geom_out.frag_position = vec3(model_matrix * gl_in[i].gl_Position);
        geom_out.frag_position_light_space = light_space_matrix * vec4(geom_out.frag_position, 1.0);
        geom_out.frag_normal = transpose(inverse(mat3(model_matrix))) * gl_in[i].gl_Position.xyz;
        geom_out.frag_texcoord = geom_in[i].geom_texcoord;

        gl_Position = projection_matrix * view_matrix * vec4(geom_out.frag_position, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}
