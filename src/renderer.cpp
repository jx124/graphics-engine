#include "renderer.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern float mixValue;

Renderer::Renderer(size_t width, size_t height) : width(width), height(height) {
    this->state = std::make_unique<RendererState>();
    this->imGuiState = std::make_unique<ImGuiState>();
    this->image = std::vector<uint8_t>(width * height * 3);

    vfov = 45.0f;
    aspectRatio = 16.0f / 9.0f;
    viewportHeight = 2.0f;
    viewportWidth = aspectRatio * viewportHeight;
    focalLength = 1.0f;

    origin = glm::vec3(0.0f, 0.0f, 0.0f);
    horizontal = glm::vec3(viewportWidth, 0.0f, 0.0f);
    vertical = glm::vec3(0.0f, viewportHeight, 0.0f);
    lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - glm::vec3(0.0f, 0.0f, focalLength);
}

size_t Renderer::setVertices(const std::vector<float> &vertices) noexcept {
    GLuint VAO, VBO;

    // Generate and bind vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    VAOs.push_back(VAO);

    // Generate and bind vertex buffer object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    VBOs.push_back(VBO);

    // Copy data into buffer memory
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // return index of location in VAOs and VBOs array.
    return VAOs.size() - 1;
}

void Renderer::addVertexAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized,
                                  GLsizei stride, size_t offset) const noexcept {
    glVertexAttribPointer(index, size, type, normalized, stride, (GLvoid *)offset);
    glEnableVertexAttribArray(index);
}

size_t Renderer::setIndices(const std::vector<uint32_t> &indices) noexcept {
    GLuint EBO;

    // Generate and bind element buffer object
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    EBOs.push_back(EBO);

    // Copy data into buffer memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // return index of location in EBOs array.
    return EBOs.size() - 1;
}

size_t Renderer::loadTexture2D(std::string_view filePath, GLint format) {
    // Initialize settings and variables
    stbi_set_flip_vertically_on_load(true);

    int width, height, nChannels;
    uint8_t *data = stbi_load(filePath.data(), &width, &height, &nChannels, 0);
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Read image
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        throw std::runtime_error("[GL Error] Failed to load texture");
    }
    stbi_image_free(data);

    textures.push_back(texture);

    // return index of location in textures array.
    return textures.size() - 1;
}

void Renderer::loadImage(const std::vector<uint8_t> &image) noexcept {
    if (imageTexture == 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &imageTexture);
        glBindTexture(GL_TEXTURE_2D, imageTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

        return;
    }

    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
}

void Renderer::renderInit() noexcept {
    // clang-format off
    std::vector<float> vertices = {
        // vertices       // texture coords
        -1.0f,  -1.0f,    0.0f, 0.0f,
         1.0f,  -1.0f,    1.0f, 0.0f,
         1.0f,   1.0f,    1.0f, 1.0f,
        -1.0f,   1.0f,    0.0f, 1.0f,
    }; // clang-format on

    setVertices(vertices);
    addVertexAttribute(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    addVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 2 * sizeof(float));

    addShader("Image", "res/vertexImage.glsl", "res/fragmentImage.glsl");

    getShader("Image").bind();
    getShader("Image").setInt("ImageTexture", 0);

    loadImage(image);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    std::cout << "Number of OpenMP threads: " << omp_get_max_threads() << "\n";

    this->scene.emplace_back(std::make_unique<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f));
    this->scene.emplace_back(std::make_unique<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f));
}

glm::vec3 inline rayColor(const Ray &ray, std::vector<std::unique_ptr<Hittable>> &scene) {
    HitRecord rec;
    bool hit = false;
    float closest = std::numeric_limits<float>::infinity();

    for (auto &object : scene) {
        if (object->hit(ray, 0.0f, closest, rec)) {
            hit = true;
            closest = rec.t;
        }
    }

    if (hit) {
        return 0.5f * (rec.normal + glm::vec3(1.0f));
    }

    glm::vec3 unitDirection = glm::normalize(ray.direction);
    float t = 0.5f * (unitDirection.y + 1.0f);
    return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
}

void Renderer::renderLoop(float time) noexcept {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    xorshift64_state rngState{static_cast<uint64_t>(time * 1234)};

    #pragma omp parallel for schedule(nonmonotonic : auto), shared(image), firstprivate(rngState)
    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            float u = (float(i) + randomFloat(&rngState)) / (width - 1);
            float v = (float(j) + randomFloat(&rngState)) / (height - 1);

            Ray r(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
            setPixelColor(i, j, rayColor(r, scene));
        }
    }

    loadImage(image);

    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::renderImGui() noexcept {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGuiIO &io = ImGui::GetIO();

    {
        ImGui::Begin("Settings");

        // ImGui::SeparatorText("Material Properties");
        // ImGui::PushItemWidth(-110);
        // ImGui::ColorEdit3("Ambient Color", (float *)&imGuiState->materialAmbient);
        // ImGui::ColorEdit3("Diffuse Color", (float *)&imGuiState->materialDiffuse);
        // ImGui::ColorEdit3("Specular Color", (float *)&imGuiState->materialSpecular);
        // ImGui::SliderFloat("Shininess", &imGuiState->materialShininess, 0.0f, 256.0f);

        // ImGui::NewLine();
        // ImGui::SeparatorText("Light Properties");
        // ImGui::ColorEdit3("Ambient Color", (float *)&imGuiState->lightAmbient);
        // ImGui::ColorEdit3("Diffuse Color", (float *)&imGuiState->lightDiffuse);
        // ImGui::ColorEdit3("Specular Color", (float *)&imGuiState->lightSpecular);
        // ImGui::PopItemWidth();

        // ImGui::NewLine();
        ImGui::Text("Framerate: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::updateLoop(float time) noexcept {
    this->resize();
    this->renderLoop(time);
    this->renderImGui();
}

void Renderer::showWireframe(bool value) const noexcept {
    glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
}

void Renderer::setViewMatrix(const glm::mat4 &view) {
}

void Renderer::addShader(const std::string &name, const char *vertexPath, const char *fragmentPath) noexcept {
    shaders[name] = std::make_unique<Shader>(vertexPath, fragmentPath);
}

const Shader &Renderer::getShader(const std::string &name) noexcept {
    return *shaders[name];
}

void Renderer::resize() noexcept {
    if (toResize) {
        image = std::vector<uint8_t>(width * height * 3, 0);
        toResize = false;
    }
}

void inline Renderer::setPixelColor(size_t i, size_t j, float r, float g, float b) noexcept {
    image[(i + j * width) * 3 + 0] = static_cast<uint8_t>(255.999 * r);
    image[(i + j * width) * 3 + 1] = static_cast<uint8_t>(255.999 * g);
    image[(i + j * width) * 3 + 2] = static_cast<uint8_t>(255.999 * b);
}

void inline Renderer::setPixelColor(size_t i, size_t j, const glm::vec3 &color) noexcept {
    image[(i + j * width) * 3 + 0] = static_cast<uint8_t>(255.999 * color.r);
    image[(i + j * width) * 3 + 1] = static_cast<uint8_t>(255.999 * color.g);
    image[(i + j * width) * 3 + 2] = static_cast<uint8_t>(255.999 * color.b);
}

bool Sphere::hit(const Ray &ray, float tMin, float tMax, HitRecord &record) const noexcept {
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float half_b = glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return false;
    } 
    
    float sqrtd = sqrt(discriminant);
    float root = (-half_b - sqrtd) / a;
    if (root < tMin || root > tMax) {
        root = (-half_b + sqrtd) / a;
        if (root < tMin || root > tMax) {
            return false;
        }
    }

    record.t = root;
    record.point = ray.at(root);
    glm::vec3 outwardNormal = (record.point - center) / radius;
    record.setFaceNormal(ray, outwardNormal);

    return true;
}
