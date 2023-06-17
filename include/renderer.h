#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>

#include <glm/glm.hpp>
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
#include <memory>
#include <unordered_map>

#include "shader.h"

struct RendererState {
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    
    glm::mat4 view;

    float lastFrameTime;
};

struct ImGuiState {
    glm::vec3 materialAmbient;
    glm::vec3 materialDiffuse;
    glm::vec3 materialSpecular;
    float materialShininess;

    glm::vec3 lightAmbient;
    glm::vec3 lightDiffuse;
    glm::vec3 lightSpecular;
};

class Renderer {
private:
    std::vector<GLuint> VAOs, VBOs, EBOs, textures;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unique_ptr<ImGuiState> imGuiState;
public:
    Renderer();
    size_t setVertices(std::vector<float> vertices);
    void addVertexAttribute(GLuint index,
                            GLint size,
                            GLenum type,
                            GLboolean normalized,
                            GLsizei stride,
                            size_t offset);
    size_t setIndices(std::vector<uint32_t> indices);
    size_t loadTexture2D(const char *filePath, GLint format);
    std::unique_ptr<RendererState> state;

    void renderInit();
    void renderLoop(float time);
    void renderImGui();
    void updateLoop(float time);
    // add void renderCleanup??
    void showWireframe(bool value);
    void setViewMatrix(const glm::mat4 &view);
    void addShader(const std::string &name, const char *vertexPath, const char *fragmentPath);
    const Shader& getShader(const std::string &name);
};
