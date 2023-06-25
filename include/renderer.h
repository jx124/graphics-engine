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
    std::vector<uint8_t> image;
    GLuint imageTexture = 0;
public:
    Renderer(size_t width, size_t height);
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
    size_t width, height;
    bool toResize = false;

    // TODO: move into camera class
    float vfov, aspectRatio, viewportHeight, viewportWidth, focalLength;
    glm::vec3 origin, horizontal, vertical, lowerLeftCorner;

    void renderInit();
    void renderLoop(float time);
    void renderImGui();
    void updateLoop(float time);
    void loadImage(const std::vector<uint8_t> &image);
    void showWireframe(bool value);
    void setViewMatrix(const glm::mat4 &view);
    void addShader(const std::string &name, const char *vertexPath, const char *fragmentPath);
    const Shader& getShader(const std::string &name);
    void resize();
    void setPixelColor(size_t i, size_t j, float r, float g, float b);
    void setPixelColor(size_t i, size_t j, const glm::vec3& color);
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
};