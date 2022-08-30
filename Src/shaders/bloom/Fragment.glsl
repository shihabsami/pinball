#version 460 core

#define DEFAULT_FBO  0
#define HDR_FBO      1
#define GAUSSIAN_FBO 2

out vec4 frag_color;

in vec2 frag_texcoord;

uniform sampler2D color_image;
uniform sampler2D hdr_image;
uniform sampler2D blurred_image;

uniform bool bloom;
uniform float exposure;
uniform int FBO_debug_index;

void main() {
    switch (FBO_debug_index) {
        case DEFAULT_FBO:
            frag_color = vec4(texture(color_image, frag_texcoord).rgb, 1.0);
            break;
        case HDR_FBO:
            frag_color = vec4(texture(hdr_image, frag_texcoord).rgb, 1.0);
            break;
        case GAUSSIAN_FBO:
            frag_color = vec4(texture(blurred_image, frag_texcoord).rgb, 1.0);
            break;
        default:
            float gamma = 2.2;
            vec3 hdr_color = texture(color_image, frag_texcoord).rgb;
            vec3 bloom_color = texture(blurred_image, frag_texcoord).rgb;
            if (bloom)
                hdr_color += bloom_color;

            vec3 result = vec3(1.0) - exp(-hdr_color * exposure);
            result = pow(result, vec3(1.0 / gamma));
            frag_color = vec4(result, 1.0);
    }
}
