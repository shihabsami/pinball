#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>

#include "../components/Mesh.h"

class Skybox {
public:
    explicit Skybox(std::vector<const char*> textures);

    void init();

    void render(std::shared_ptr<Shader>& shader) const;

    void done();

    unsigned int VAO{}, VBO{};
    unsigned int m_cubemap{};
    std::vector<glm::vec3> m_vertex_positions;
    std::vector<const char*> m_textures;
};

#endif // SKYBOX_H
