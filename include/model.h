#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iterator>

#include "mesh.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
    Model(std::string path) { load_model(path); }
    void draw(const Shader& shader);

private:
    void load_model(std::string path);
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);

    std::vector<Mesh> meshes;
    std::vector<Texture> loaded_textures;
    std::string directory;
};
