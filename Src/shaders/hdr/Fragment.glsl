#version 460 core

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 hdr_color;

#define MAX_LIGHTS     20
#define DIRECTIONAL    0
#define POINT          1
#define SPOT           2

struct Light {
    int type;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    float cutoff;
    float outer_cutoff;
};

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
    bool has_diffuse_map;
    bool has_specular_map;
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

in Data {
    vec3 frag_position;
    vec4 frag_position_light_space;
    vec3 frag_normal;
    vec2 frag_texcoord;
} frag_in;

uniform int light_count;
uniform Light lights[MAX_LIGHTS];
uniform Material material;
uniform vec3 view_position;

void main() {
    if (light_count == 0) {
        frag_color = vec4(material.has_diffuse_map
            ? texture(material.diffuse_map, frag_in.frag_texcoord).rgb
            : material.diffuse_color,
        1.0);
    } else {
//        vec3 color = vec3(5.0, 0.0, 0.0);
//        vec3 N = normalize(frag_in.frag_normal);
//        vec3 final_color = vec3(0.0);
//        vec3 lighting = vec3(0.0);
//        vec3 ambient = 0.0 * color;
//
//        for (int i = 0; i < num_lights; i++) {
//            // diffuse
//            vec3 L = normalize(lights[i].position - frag_in.frag_position);
//            float diff = max(dot(L, N), 0.0);
//            vec3 result = lights[i].diffuse * diff * color;
//
//            // attenuation (use quadratic as we have gamma correction)
//            float distance = length(frag_in.frag_position - lights[i].position);
//            result *= 1.0 / (distance * distance);
//            lighting += result;
//        }
//
//        final_color = ambient + lighting;

        vec3 N = normalize(frag_in.frag_normal);
        vec3 final_color = vec3(0.0, 0.0, 0.0);

        for (int i = 0; i < light_count; i++) {
            float attenuation = 1.0;

            // ambient
            vec3 ambient = lights[i].ambient * (material.has_diffuse_map
            ? texture(material.diffuse_map, frag_in.frag_texcoord).rgb
            : material.diffuse_color);

            // diffuse
            vec3 L;
            if (lights[i].type == DIRECTIONAL) {
                L = -lights[i].direction;
            } else {
                L = lights[i].position - frag_in.frag_position;
                float Ld = length(L);
                attenuation = min(1.0 / (
                (lights[i].constant) +
                (lights[i].linear * Ld) +
                (lights[i].quadratic * Ld * Ld)
                ), 1);
            }
            L = normalize(L);
            float d = max(dot(N, L), 0.0);
            vec3 diffuse = lights[i].diffuse * d * (material.has_diffuse_map
            ? texture(material.diffuse_map, frag_in.frag_texcoord).rgb
            : material.diffuse_color);

            // specular
            vec3 V = normalize(view_position - frag_in.frag_position);
            vec3 H = normalize(L + V);
            float s = pow(max(dot(N, H), 0.0), material.shininess);
            vec3 specular = lights[i].specular * s * (material.has_specular_map
            ? texture(material.specular_map, frag_in.frag_texcoord).rgb
            : material.specular_color);

            final_color += (ambient + attenuation * (diffuse + specular));
        }

        float brightness = dot(final_color, vec3(0.2126, 0.7152, 0.0722));
        if (brightness > 1.0)
            hdr_color = vec4(final_color, 1.0);
        else
            hdr_color = vec4(0.0, 0.0, 0.0, 1.0);
        frag_color = vec4(final_color, 1.0);
    }
}
