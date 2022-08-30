#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include "Material.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

struct Vertex {
    Vertex() : position(glm::vec3()), normal(glm::vec3()), texcoord(glm::vec2()) {}

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texcoord)
        : position(position), normal(normal), texcoord(texcoord) {}

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class Mesh {
public:
    Mesh();

    Mesh(
        std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        Material material);

    virtual void init();

    virtual void done();

    virtual void render();

    virtual void render(std::shared_ptr<Shader>& shader);

    unsigned int VAO{}, VBO{}, EBO{};
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    Material m_material;
};

#endif // MESH_H
