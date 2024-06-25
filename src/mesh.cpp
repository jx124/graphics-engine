#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
    // TODO: should the constructor arguments be passed by value?
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setup_mesh();
}

Mesh::Mesh(const MeshData& mesh_data) : Mesh(mesh_data.vertices, mesh_data.indices, mesh_data.textures) {}

void Mesh::setup_mesh() {
    this->VAO.bind();
    this->VBO.bind();
    this->EBO.bind();

    this->VBO.write_buffer_data(this->vertices, GL_STATIC_DRAW);
    this->EBO.write_buffer_data(this->indices, GL_STATIC_DRAW);
    
    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
    glEnableVertexAttribArray(2);
}

void Mesh::draw(const Shader& shader) {
    GLuint n_diffuse = 1;
    GLuint n_specular = 1;
    GLuint n_normal = 1;
    GLuint n_height = 1;

    for (size_t i = 0; i < this->textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(n_diffuse++);
        } else if (name == "texture_specular") {
            number = std::to_string(n_specular++);
        } else if (name == "texture_normal") {
            number = std::to_string(n_normal++);
        } else if (name == "texture_height") {
            number = std::to_string(n_height++);
        }

        shader.set(name + number, static_cast<int>(i));
        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    this->VAO.bind();
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
}

MeshData Mesh::generate_cube_mesh() {
    constexpr float cube_values[] = {
        // vertex position,  vertex normal,       tex coords
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < 36; i++) {
        vertices.push_back({
            glm::vec3(cube_values[8 * i + 0], cube_values[8 * i + 1], cube_values[8 * i + 2]),
            glm::vec3(cube_values[8 * i + 3], cube_values[8 * i + 4], cube_values[8 * i + 5]),
            glm::vec2(cube_values[8 * i + 6], cube_values[8 * i + 7])
        });
        indices.push_back(i);
    }
    return {vertices, indices, {}};
}

MeshData Mesh::generate_plane_mesh() {
    // generates a 1x1 xz-plane centered at the origin

    constexpr float plane_values[] = {
        // vertex position,  vertex normal,       tex coords
       -0.5f, 0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, 0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, 0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, 0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f, 0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f, 0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    };

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < 6; i++) {
        vertices.push_back({
            glm::vec3(plane_values[8 * i + 0], plane_values[8 * i + 1], plane_values[8 * i + 2]),
            glm::vec3(plane_values[8 * i + 3], plane_values[8 * i + 4], plane_values[8 * i + 5]),
            glm::vec2(plane_values[8 * i + 6], plane_values[8 * i + 7])
        });
        indices.push_back(i);
    }
    return {vertices, indices, {}};
}