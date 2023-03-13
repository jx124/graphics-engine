#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "window.cpp"

int main() {
    Window window = Window(800, 600, "Graphics Engine");
    bool success = window.createWindow();

    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    window.setVertices(vertices);
    window.loadShader("res/fragment.glsl", Window::ShaderType::Fragment);
    window.loadShader("res/vertex.glsl", Window::ShaderType::Vertex);
    window.compileShaders();

    if (success) {
        window.run();
    }
}