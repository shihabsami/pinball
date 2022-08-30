#version 460 core

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 hdr_color;

uniform sampler2D particle_texture;

in Data {
    vec2 frag_texcoord;
} frag_in;

void main() {
    vec4 texture_color = texture(particle_texture, frag_in.frag_texcoord);
    if(texture_color.a < 0.1)
        discard;

    frag_color = texture_color;
    float brightness = dot(texture_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        hdr_color = vec4(texture_color.rgb, 1.0);
    else
        hdr_color = vec4(0.0, 0.0, 0.0, 1.0);
}
