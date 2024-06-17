#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
    // TODO: should the constructor arguments be passed by value?
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setup_mesh();
}

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

    // draw mesh
    this->VAO.bind();
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
}
