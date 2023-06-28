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
#include <limits>

#include <omp.h>

#include "sampler.h"
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

// TODO: abstract away properly

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
    glm::vec3 at(float t) const noexcept { return origin + t * direction; }
};

struct HitRecord {
    glm::vec3 point;
    glm::vec3 normal;
    float t;
    bool frontFace;

    inline void setFaceNormal(const Ray &ray, const glm::vec3 &outwardNormal) noexcept {
        frontFace = glm::dot(ray.direction, outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual bool hit(const Ray &ray, float tMin, float tMax, HitRecord &record) const noexcept = 0;
    virtual ~Hittable() = default;
};

class Sphere : public Hittable {
public:
    Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {}
    virtual bool hit(const Ray &ray, float tMin, float tMax, HitRecord &record) const noexcept override;

    glm::vec3 center;
    float radius;
};

class Renderer {
private:
    std::vector<GLuint> VAOs, VBOs, EBOs, textures;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unique_ptr<ImGuiState> imGuiState;
    std::vector<uint8_t> image;
    GLuint imageTexture = 0;
    std::vector<std::unique_ptr<Hittable>> scene;
    std::vector<float> accumulator;
    size_t count;
    bool accumulate = true;

public:
    Renderer(size_t width, size_t height);
    size_t setVertices(const std::vector<float> &vertices) noexcept;
    void addVertexAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized,
                            GLsizei stride, size_t offset) const noexcept;
    size_t setIndices(const std::vector<uint32_t> &indices) noexcept;
    size_t loadTexture2D(std::string_view filePath, GLint format);
    std::unique_ptr<RendererState> state;
    size_t width, height;
    bool toResize = false;

    // TODO: move into camera class
    float vfov, aspectRatio, viewportHeight, viewportWidth, focalLength;
    glm::vec3 origin, horizontal, vertical, lowerLeftCorner;

    void renderInit() noexcept;
    void renderLoop(float time) noexcept;
    void renderImGui() noexcept;
    void updateLoop(float time) noexcept;
    void loadImage(const std::vector<uint8_t> &image) noexcept;
    void showWireframe(bool value) const noexcept;
    void setViewMatrix(const glm::mat4 &view);
    // TODO: change const char* here and in Shader class
    void addShader(const std::string &name, const char *vertexPath, const char *fragmentPath) noexcept; 
    const Shader &getShader(const std::string &name) noexcept;
    void resize() noexcept;
    void setPixelColor(size_t i, size_t j, float r, float g, float b) noexcept;
    void setPixelColor(size_t i, size_t j, const glm::vec3 &color) noexcept;
    void accumulatePixel(size_t i, size_t j, float r, float g, float b) noexcept;
    void accumulatePixel(size_t i, size_t j, const glm::vec3 &color) noexcept;
};
