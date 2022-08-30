#version 460 core

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 hdr_color;

uniform vec3 diffuse_color;

void main() {
    frag_color = vec4(diffuse_color, 1.0);
    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        hdr_color = vec4(diffuse_color.rgb, 1.0);
    else
        hdr_color = vec4(0.0, 0.0, 0.0, 1.0);
}
