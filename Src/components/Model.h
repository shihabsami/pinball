#ifndef MODEL_H
#define MODEL_H

#include "Shader.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

class Model : public std::enable_shared_from_this<Model> {
public:
    Model() = default;

    explicit Model(const char* path);

    void init();

    void render();

    void render(std::shared_ptr<Shader>& shader);

    void done();

    void load();

    void process_node(aiNode* node, const aiScene* scene);

    Mesh process_mesh(aiMesh* mesh, const aiScene* scene) const;

    Material process_material(aiMaterial* material) const;

    std::vector<Mesh> m_meshes;
    std::string m_path;
    std::string m_directory;
};

#endif // MODEL_H
