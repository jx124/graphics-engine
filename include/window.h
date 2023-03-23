#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <cmath>

#include "renderer.h"

class Window {
private:
    uint32_t width, height;
    const char *windowName;
    GLFWwindow *window;

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void processInput(GLFWwindow *window);
    static void GLAPIENTRY debugMessageCallback(GLenum source,
                                                GLenum type,
                                                GLuint id,
                                                GLenum severity,
                                                GLsizei length,
                                                const GLchar *message,
                                                const void *userParam);
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> VAOs, VBOs, EBOs, shaders;
    uint32_t VAO, VBO, EBO, vertexShader, fragmentShader, shaderProgram;
    const char *vertexShaderSource;
    const char *fragmentShaderSource;

    std::unique_ptr<Renderer> renderer;

public:
    Window(uint32_t width, uint32_t height, const char *windowName);
    ~Window();

    enum ShaderType {
        Vertex,
        Fragment
    };

    bool createWindow();
    void setVertices(std::vector<float> vertices);
    void addVertexAttribute(GLuint index,
                            GLint size,
                            GLenum type,
                            GLboolean normalized,
                            GLsizei stride,
                            size_t offset);
    void setIndices(std::vector<uint32_t> indices);
    void createShaderProgram(const char *vertexPath, const char* fragmentPath);
    void loadShader(const char *filePath, Window::ShaderType type);
    uint32_t loadTexture(const char *filePath, GLenum textureUnit, GLint format);
    void compileShaders();
    void render();
    void showWireframe(bool value);
    void run();
};
