#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>

#include <glm/glm.hpp>
// #include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>

class Renderer {
private:
    std::vector<GLuint> VAOs, VBOs, EBOs, shaderPrograms, textures;

public:
    size_t setVertices(std::vector<float> vertices);
    void addVertexAttribute(GLuint index,
                            GLint size,
                            GLenum type,
                            GLboolean normalized,
                            GLsizei stride,
                            size_t offset);
    size_t setIndices(std::vector<uint32_t> indices);
    size_t createShaderProgram(const char *vertexPath, const char *fragmentPath);
    size_t loadTexture2D(const char *filePath, GLint format);
    void renderInit();
    void renderLoop(float time);
    void renderImGui(ImGuiIO &io);
    // add void renderCleanup??
    void showWireframe(bool value);
    void setViewMatrix(const glm::mat4 &view);
};
