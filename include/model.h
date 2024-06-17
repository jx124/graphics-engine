#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "mesh.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct MaterialType {
    aiMaterial* material;
    aiTextureType type;
    std::string type_name;
};

class Model {
public:
    Model(std::string path) { load_model(path); }
    void draw(const Shader& shader);

private:
    void load_model(std::string path);
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> load_material_textures(const std::vector<MaterialType>& material_types);

    std::vector<Mesh> meshes;
    std::vector<Texture> loaded_textures;
    std::string directory;
};
