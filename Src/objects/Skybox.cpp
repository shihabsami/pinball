#include "Skybox.h"

#include "../utility/Loader.h"

Skybox::Skybox(std::vector<const char*> textures) : m_textures(std::move(textures)) {
    m_vertex_positions = std::vector<glm::vec3>({
        { -1.0f, 1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },

        { -1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },

        { 1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },

        { -1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },

        { -1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, -1.0f },

        { -1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f }
    });
}

void Skybox::init() {
    m_cubemap = load_cubemap(m_textures);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        (GLsizeiptr) (m_vertex_positions.size() * sizeof(glm::vec3)), m_vertex_positions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skybox::render(std::shared_ptr<Shader>& shader) const {
    glDepthFunc(GL_LEQUAL);
    shader->use();

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) m_vertex_positions.size());
    glActiveTexture(0);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

void Skybox::done() {
    // Delete all the objects
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
