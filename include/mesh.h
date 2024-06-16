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

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
    void draw(const Shader& shader);

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

private:
    void setup_mesh();

    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;
};

