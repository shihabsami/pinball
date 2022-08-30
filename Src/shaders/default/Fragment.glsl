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
    bool shadows;
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

uniform vec3 view_position;
uniform sampler2D depth_map_dl;
uniform samplerCube depth_map_odl;
uniform int depth_map_visualiser_index;
uniform bool visualise_hdr;
uniform float far_plane;
uniform int light_count;
uniform Light lights[MAX_LIGHTS];
uniform Material material;

void main() {
    vec4 depth_color = vec4(0.0);
    bool depth_color_set = false;

    if (light_count == 0) {
        frag_color = vec4(material.has_diffuse_map
            ? texture(material.diffuse_map, frag_in.frag_texcoord).rgb
            : material.diffuse_color,
        1.0);
    } else {
        vec3 N = normalize(frag_in.frag_normal);
        vec3 final_color = vec3(0.0, 0.0, 0.0);

        for (int i = 0; i < light_count; i++) {
            // ambient
            vec3 ambient = lights[i].ambient * (material.has_diffuse_map
                ? texture(material.diffuse_map, frag_in.frag_texcoord).rgb
                : material.diffuse_color);

            // diffuse
            float attenuation = 1.0;
            vec3 L;
            if (lights[i].type == DIRECTIONAL) {
                L = -lights[i].direction;
            } else {
                L = lights[i].position - frag_in.frag_position;
                float Ld = length(L);
                attenuation = min(
                    1.0 / (
                    (lights[i].constant) +
                    (lights[i].linear * Ld) +
                    (lights[i].quadratic * Ld * Ld)
                    ), 1
                );
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

            // shadow
            float shadow = 0.0;
            if (lights[i].type == DIRECTIONAL) {
                vec3 frag_position_transformed = frag_in.frag_position_light_space.xyz / frag_in.frag_position_light_space.w
                * 0.5 + 0.5;

                if (frag_position_transformed.z <= 1.0) {
                    float closest_depth = texture(depth_map_dl, frag_position_transformed.xy).r;
                    float current_depth = frag_position_transformed.z;

                    vec3 normal = normalize(frag_in.frag_normal);
                    vec3 light_direction = normalize(lights[i].position - frag_in.frag_position);
                    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);

                    vec2 texel_size = 1.0 / textureSize(depth_map_dl, 0);
                    for (int x = -1; x <= 1; ++x) {
                        for (int y = -1; y <= 1; ++y) {
                            float pcf_depth = texture(depth_map_dl, frag_position_transformed.xy + vec2(x, y) * texel_size).r;
                            shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;
                        }
                    }
                    shadow /= 9.0;
                }
            } else if (lights[i].shadows) {
                vec3 frag_to_light = frag_in.frag_position - lights[i].position;
                depth_color = vec4(vec3(texture(depth_map_odl, frag_to_light).r), 1.0);
                depth_color_set = true;

                float current_depth = length(frag_to_light);
                float bias = 0.05;
                float samples = 4.0;
                float offset = 0.01;
                for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
                    for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
                        for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                            float closest_depth = texture(depth_map_odl, frag_to_light + vec3(x, y, z)).r;
                            closest_depth *= far_plane;
                            if (current_depth - bias > closest_depth)
                            shadow += 1.0;
                        }
                    }
                }
                shadow /= (samples * samples * samples);
            }

            final_color += (ambient + (1.0 - shadow) * (diffuse + specular));
        }

        // for visualising depth map
        if (depth_map_visualiser_index == POINT && depth_color_set) {
            frag_color = depth_color;
        } else {
            frag_color = vec4(final_color, 1.0);
        }

        float brightness = dot(final_color, vec3(0.2126, 0.7152, 0.0722));
        if (brightness > 1.0)
            hdr_color = vec4(final_color, 1.0);
        else
            hdr_color = vec4(0.0, 0.0, 0.0, 1.0);

        if (visualise_hdr) {
            frag_color = hdr_color;
        }
    }
}
