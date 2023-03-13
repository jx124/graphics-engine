#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "window.cpp"

int main() {
    Window window = Window(800, 600, "Graphics Engine");
    bool success = window.createWindow();
    if (!success) {
        return 1;
    }

    // clang-format off
    std::vector<float> vertices = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    std::vector<uint32_t> indices = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    // clang-format on

    window.setVertices(vertices);
    window.setIndices(indices);
    window.loadShader("res/fragment.glsl", Window::ShaderType::Fragment);
    window.loadShader("res/vertex.glsl", Window::ShaderType::Vertex);
    window.compileShaders();

    window.run();
}