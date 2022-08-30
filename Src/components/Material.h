#ifndef MATERIAL_H
#define MATERIAL_H

struct Texture {
    unsigned int id{};
    std::string type{};
};

struct Material {
    glm::vec3 diffuse_color{};
    glm::vec3 specular_color{};
    bool has_diffuse_map{};
    bool has_specular_map{};
    Texture diffuse_map{};
    Texture specular_map{};
    float shininess{};
};

#endif // MATERIAL_H
