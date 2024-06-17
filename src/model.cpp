#include "model.h"

void Model::draw(const Shader& shader) {
    for (size_t i = 0; i < this->meshes.size(); i++) {
        this->meshes[i].draw(shader);
    }
}

void Model::load_model(std::string path) {
    std::cout << "[Assimp] Importing model..." << std::endl;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "[Assimp] Error importing model: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "[Assimp] Imported model " << path << std::endl;

    this->directory = path.substr(0, path.find_last_of('/'));

    auto start = std::chrono::high_resolution_clock::now();

    this->process_node(scene->mRootNode, scene);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[Assimp] Processing all nodes took " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms" << std::endl;
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    std::cout << "[Assimp] Processing node " << node->mName.C_Str() << std::endl;

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

    std::vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(),
            std::make_move_iterator(diffuse_maps.begin()),
            std::make_move_iterator(diffuse_maps.end()));

    std::vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(),
            std::make_move_iterator(specular_maps.begin()),
            std::make_move_iterator(specular_maps.end()));

    std::vector<Texture> normal_maps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(),
            std::make_move_iterator(normal_maps.begin()),
            std::make_move_iterator(normal_maps.end()));

    std::vector<Texture> height_maps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(),
            std::make_move_iterator(height_maps.begin()),
            std::make_move_iterator(height_maps.end()));

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str); // retrieve texture file location
        bool skip = false;

        for (size_t j = 0; j < this->loaded_textures.size(); j++) {
            // TODO: check if unordered_map is faster
            if (std::strcmp(this->loaded_textures[i].path.data(), str.C_Str()) == 0) {
                textures.push_back(this->loaded_textures[j]);
                skip = true;
                break;
            }
        }
        
        if (!skip) {
            Texture texture(this->directory + "/" + str.C_Str());
            texture.type = type_name;
            texture.path = str.C_Str();

            textures.push_back(texture);
            this->loaded_textures.push_back(texture);
        }
    }

    return textures;
}
