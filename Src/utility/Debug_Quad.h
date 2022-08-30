#ifndef QUAD_H
#define QUAD_H

#include <glad/glad.h>

class Debug_Quad {
public:
    void init() {
        // Positions and texcoords
        float vertices[] = {
            -1.0f, 1.0f, 0.0f,
            0.0f, 1.0f,

            -1.0f, -1.0f, 0.0f,
            0.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f,

            1.0f, -1.0f, 0.0f,
            1.0f, 0.0f,
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void render() const {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void done() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    unsigned int VAO, VBO;
};

#endif // QUAD_H
