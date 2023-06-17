#include "renderer.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern float mixValue;

Renderer::Renderer() {
    this->state = std::make_unique<RendererState>();
    this->imGuiState = std::make_unique<ImGuiState>();
}

size_t Renderer::setVertices(std::vector<float> vertices) {
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

void Renderer::addVertexAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset) {
    glVertexAttribPointer(index, size, type, normalized, stride, (GLvoid *)offset);
    glEnableVertexAttribArray(index);
}

size_t Renderer::setIndices(std::vector<uint32_t> indices) {
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

size_t Renderer::loadTexture2D(const char *filePath, GLint format) {
    // Initialize settings and variables
    stbi_set_flip_vertically_on_load(true);

    int width, height, nChannels;
    unsigned char *data = stbi_load(filePath, &width, &height, &nChannels, 0);
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
        std::cerr << "[GL Error] Failed to load texture" << std::endl;
        return 0;
    }
    stbi_image_free(data);

    textures.push_back(texture);

    // return index of location in textures array.
    return textures.size() - 1;
}

void Renderer::renderInit() {
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // clang-format off
    std::vector<float> vertices1 = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
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
        2, 3, 0    // second triangle
    };

    std::vector<uint32_t> indices2 = {
        0, 1, 2,   // first triangle
        1, 3, 4    // second triangle
    };
    

    std::vector<float> vertices = {
        // vertices           // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    setVertices(vertices); // add move assignment?
    addVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    addVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3 * sizeof(float));
    addVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 6 * sizeof(float));

    addShader("Box", "res/vertexLight.glsl", "res/fragmentLight.glsl");
    addShader("Light", "res/vertexLight.glsl", "res/fragmentLightSource.glsl");

    loadTexture2D("res/container.jpg", GL_RGB);
    loadTexture2D("res/awesomeface.png", GL_RGBA);

    getShader("Box").bind();
    getShader("Box").setInt("texture1", 0);
    getShader("Box").setInt("texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    getShader("Box").bind();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    getShader("Box").setMat4("projection", projection);
    getShader("Box").setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));

    // light cube
    setVertices(vertices);
    addVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    getShader("Light").bind();
    getShader("Light").setMat4("projection", projection);

    this->imGuiState->materialAmbient = glm::vec3(1.0f, 0.5f, 0.31f);
    this->imGuiState->materialDiffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    this->imGuiState->materialSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
    this->imGuiState->materialShininess = 32.0f;
    this->imGuiState->lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    this->imGuiState->lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    this->imGuiState->lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
}

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)};

void Renderer::renderLoop(float time) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Shader &boxShader = getShader("Box");
    const Shader &lightShader = getShader("Light");

    boxShader.bind();
    boxShader.setFloat("mixValue", mixValue);
    boxShader.setVec3("viewPos", state->cameraPos);
    boxShader.setVec3("material.ambient", imGuiState->materialAmbient);
    boxShader.setVec3("material.diffuse", imGuiState->materialDiffuse);
    boxShader.setVec3("material.specular", imGuiState->materialSpecular);
    boxShader.setFloat("material.shininess", imGuiState->materialShininess);
    boxShader.setVec3("light.ambient", imGuiState->lightAmbient);
    boxShader.setVec3("light.diffuse", imGuiState->lightDiffuse); // darken diffuse light a bit
    boxShader.setVec3("light.specular", imGuiState->lightSpecular);

    glBindVertexArray(VAOs[0]);
    for (size_t i = 0; i < 10; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
        model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        boxShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // light cube
    lightShader.bind();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.2f, 1.0f, 2.0f));
    model = glm::scale(model, glm::vec3(0.2f));

    lightShader.setMat4("model", model);
    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::renderImGui() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGuiIO &io = ImGui::GetIO();

    {
        ImGui::Begin("Settings");

        ImGui::SeparatorText("Material Properties");
        ImGui::PushItemWidth(-110);
        ImGui::ColorEdit3("Ambient Color", (float *)&imGuiState->materialAmbient);
        ImGui::ColorEdit3("Diffuse Color", (float *)&imGuiState->materialDiffuse);
        ImGui::ColorEdit3("Specular Color", (float *)&imGuiState->materialSpecular);
        ImGui::SliderFloat("Shininess", &imGuiState->materialShininess, 0.0f, 256.0f);

        // ImGui::NewLine();
        ImGui::SeparatorText("Light Properties");
        ImGui::ColorEdit3("Ambient Color", (float *)&imGuiState->lightAmbient);
        ImGui::ColorEdit3("Diffuse Color", (float *)&imGuiState->lightDiffuse);
        ImGui::ColorEdit3("Specular Color", (float *)&imGuiState->lightSpecular);
        ImGui::PopItemWidth();

        // ImGui::NewLine();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::updateLoop(float time) {
    this->setViewMatrix(this->state->view);
    this->renderLoop(time);
    this->renderImGui();
}

void Renderer::showWireframe(bool value) {
    glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
}

void Renderer::setViewMatrix(const glm::mat4 &view) {
    getShader("Box").bind();
    getShader("Box").setMat4("view", view);
    getShader("Light").bind();
    getShader("Light").setMat4("view", view);
}

void Renderer::addShader(const std::string &name, const char *vertexPath, const char *fragmentPath) {
    shaders[name] = std::make_unique<Shader>(vertexPath, fragmentPath);
}

const Shader &Renderer::getShader(const std::string &name) {
    return *shaders[name];
}