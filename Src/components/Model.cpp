#include "Model.h"

#include "Model_Library.h"
#include "../utility/Loader.h"

Model::Model(const char* path) : m_path(path) {}

void Model::init() { load(); }

void Model::render() {
    for (Mesh& mesh: m_meshes)
        mesh.render();
}

void Model::render(std::shared_ptr<Shader>& shader) {
    for (Mesh& mesh: m_meshes)
        mesh.render(shader);
}

void Model::done() {
    for (Mesh& mesh: m_meshes)
        mesh.done();
}

void Model::load() {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(m_path.c_str(),
        aiProcess_Triangulate | aiProcess_FlipUVs);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "ERROR: Error loading model: %s\n", importer.GetErrorString());
        return;
    }

    m_directory = m_path.substr(0, m_path.find_last_of('/'));
    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, scene));
    }

    // recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i], scene);
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) const {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    vertices.resize(mesh->mNumVertices);

    // iterate through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        glm::vec3 vector;

        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        // texture coordinates
        if (mesh->HasTextureCoords(0)) {
            glm::vec2 texcoord;
            texcoord.x = mesh->mTextureCoords[0][i].x;
            texcoord.y = mesh->mTextureCoords[0][i].y;
            vertex.texcoord = texcoord;
        } else {
            vertex.texcoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.at(i) = vertex;
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process mesh's material
    Material material = process_material(scene->mMaterials[mesh->mMaterialIndex]);

    Mesh mesh_processed{ vertices, indices, material };
    mesh_processed.init();
    return mesh_processed;
}

Material Model::process_material(aiMaterial* material) const {
    auto load_texture_type = [&](aiTextureType texture_type, const std::string& type) {
        if (material->GetTextureCount(texture_type) > 1)
            fprintf(stderr, "WARNING: Material %s had more than 1 texture of type \"%s\"",
                material->GetName().C_Str(), type.c_str());

        aiString path;
        material->GetTexture(texture_type, 0, &path);
        Texture texture;
        texture.id = load_texture(path.C_Str(), this->m_directory, true);
        texture.type = type;
        return texture;
    };

    Material material_processed{};
    if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
        material_processed.diffuse_map = load_texture_type(aiTextureType_DIFFUSE, "diffuse");
        material_processed.has_diffuse_map = true;
    } else {
        aiColor3D color(1.0, 1.0, 1.0);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material_processed.diffuse_color = glm::vec3{color.r, color.g, color.b};
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) != 0) {
        material_processed.specular_map = load_texture_type(aiTextureType_SPECULAR, "specular");
        material_processed.has_specular_map = true;
    } else {
        aiColor3D color(1.0, 1.0, 1.0);
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material_processed.specular_color = glm::vec3{color.r, color.g, color.b};
    }
    material->Get(AI_MATKEY_SHININESS, material_processed.shininess);
    material_processed.shininess *= 0.128f;

    return material_processed;
}
