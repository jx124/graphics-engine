#pragma once

#include "vertexarray.h"
#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"

#include <vector>
#include <string>

class CubeMap {
public:
    CubeMap() = default;
    CubeMap(const std::vector<std::string>& faces, int texture_unit = 0);

    void draw(const glm::mat4& view, const glm::mat4& projection) const;

    VertexArray VAO;
    Texture texture;
    Shader shader;
};
