#pragma once

#include "vertexarray.h"
#include "vertexbuffer.h"
#include "elementbuffer.h"
#include "shader.h"
#include "texture.h"

#include <vector>
#include <string>
#include <cstddef>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
    Mesh(const MeshData& mesh_data);
    void draw(const Shader& shader) const;

    static MeshData generate_cube_mesh();
    static MeshData generate_plane_mesh();

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

private:
    void setup_mesh();

    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;
};

