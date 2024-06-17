#include "model.h"

void Model::draw(const Shader& shader) {
    for (size_t i = 0; i < this->meshes.size(); i++) {
        this->meshes[i].draw(shader);
    }
}

void Model::load_model(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error importing model: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "Imported model " << path << std::endl;

    this->directory = path.substr(0, path.find_last_of('/'));

    auto start = std::chrono::high_resolution_clock::now();

    this->process_node(scene->mRootNode, scene);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Processing all nodes took " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms" << std::endl;
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    // process node's meshes first
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(process_mesh(mesh, scene));
    }

    // recursively process child meshes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        this->process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    vertices.reserve(mesh->mNumVertices);

    // process vertex positions, normals, and texture coordinates
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        // check if there are any texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.tex_coords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        } else {
            vertex.tex_coords = { 0.0f, 0.0f };
        }

        vertices.push_back(std::move(vertex));
    }
    indices.reserve(mesh->mNumFaces * 3);

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        // face will always be triangles due to triangulate option
        const aiFace& face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // collect all material types for loading
    std::vector<MaterialType> material_types;
    material_types.push_back({material, aiTextureType_DIFFUSE, "texture_diffuse"});
    material_types.push_back({material, aiTextureType_SPECULAR, "texture_specular"});
    material_types.push_back({material, aiTextureType_HEIGHT, "texture_normal"});
    material_types.push_back({material, aiTextureType_AMBIENT, "texture_height"});

    std::vector<Texture> texture_maps = load_material_textures(material_types);
    textures.insert(textures.end(), texture_maps.begin(), texture_maps.end());

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(const std::vector<MaterialType>& material_types) {
    std::vector<Texture> textures;
    std::vector<std::string> unloaded_textures, unloaded_types, unloaded_paths;

    for (const auto& [material, type, type_name] : material_types) {
        // get all textures associated with current material
        for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
            aiString texture_path;
            material->GetTexture(type, i, &texture_path); // retrieve texture file location
            bool skip = false;

            for (size_t j = 0; j < this->loaded_textures.size(); j++) {
                // check if texture already loaded
                if (std::strcmp(this->loaded_textures[j].path.data(), texture_path.C_Str()) == 0) {
                    textures.push_back(this->loaded_textures[j]);
                    skip = true;
                    break;
                }
            }

            // if texture not loaded, add to pool of unloaded textures
            if (!skip) {
                unloaded_textures.emplace_back(this->directory + "/" + texture_path.C_Str());
                unloaded_types.push_back(type_name);
                unloaded_paths.emplace_back(texture_path.C_Str());
            }
        }
    }

    // load all unloaded textures together asynchronously for speed up
    std::vector<Texture> new_textures = Texture::LoadTextures(unloaded_textures);
    for (size_t i = 0; i < new_textures.size(); i++) {
        new_textures[i].type = unloaded_types[i];
        new_textures[i].path = unloaded_paths[i];
    }

    textures.insert(textures.end(), new_textures.begin(), new_textures.end());
    this->loaded_textures.insert(this->loaded_textures.end(), new_textures.begin(), new_textures.end());

    return textures;
}
