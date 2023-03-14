#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "window.h"

int main() {
    Window window = Window(800, 600, "Graphics Engine");
    bool success = window.createWindow();
    if (!success) {
        return 1;
    }

    // clang-format off
    std::vector<float> vertices1 = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    std::vector<float> vertices2 = {
        -1.0f, -0.5f, 0.0f,  // bottom left
         0.0f, -0.5f, 0.0f,  // bottom midddle
        -0.5f,  0.5f, 0.0f,  // top left
         1.0f, -0.5f, 0.0f,  // bottom right
         0.5f,  0.5f, 0.0f,  // top right
    };

    std::vector<uint32_t> indices1 = {
        0, 1, 2,   // first triangle
        // 1, 2, 3    // second triangle
    };

    std::vector<uint32_t> indices2 = {
        0, 1, 2,   // first triangle
        1, 3, 4    // second triangle
    };
    // clang-format on

    window.setVertices(vertices1);
    window.addVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    window.addVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 3 * sizeof(float));
    window.setIndices(indices1);
    window.loadShader("res/fragment.glsl", Window::ShaderType::Fragment);
    window.loadShader("res/vertex.glsl", Window::ShaderType::Vertex);
    window.compileShaders();

    // window.setVertices(vertices2);
    // window.setIndices(indices2);
    // window.loadShader("res/fragmentRed.glsl", Window::ShaderType::Fragment);
    // window.loadShader("res/vertex.glsl", Window::ShaderType::Vertex);
    // window.compileShaders();

    window.run();
}