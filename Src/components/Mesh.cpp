#include "Mesh.h"

Mesh::Mesh() :
    m_vertices(std::vector<Vertex>()),
    m_indices(std::vector<unsigned int>()),
    m_material() {}

Mesh::Mesh(
    std::vector<Vertex> vertices,
    std::vector<unsigned int> indices,
    Material material) :
    m_vertices(std::move(vertices)),
    m_indices(std::move(indices)),
    m_material(std::move(material)) {}

void Mesh::init() {
    // Generating object handles for the mesh
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (m_vertices.size() * sizeof(Vertex)), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (m_indices.size() * sizeof(unsigned int)), m_indices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texcoord));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::render(std::shared_ptr<Shader>& shader) {
    if (m_material.has_diffuse_map) {
        glActiveTexture(GL_TEXTURE0 + DIFFUSE_MAP);
        glBindTexture(GL_TEXTURE_2D, m_material.diffuse_map.id);
    }
    if (m_material.has_specular_map) {
        glActiveTexture(GL_TEXTURE0 + SPECULAR_MAP);
        glBindTexture(GL_TEXTURE_2D, m_material.specular_map.id);
    }
    shader->use();
    shader->set_material("material", m_material);

    // Render mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(0);
}

void Mesh::render() {
    glColor3fv(glm::value_ptr(m_material.diffuse_color));
    glBegin(GL_TRIANGLES);
    for (const auto& index:m_indices) {
        glNormal3fv(glm::value_ptr(m_vertices.at(index).normal));
        glTexCoord2fv(glm::value_ptr(m_vertices.at(index).texcoord));
        glVertex3fv(glm::value_ptr(m_vertices.at(index).position));
    }
    glEnd();
}

void Mesh::done() {
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
