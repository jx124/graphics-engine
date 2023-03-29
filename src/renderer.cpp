#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern float mixValue;

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

size_t Renderer::createShaderProgram(const char *vertexPath, const char *fragmentPath) {
    // Open shader files
    std::ostringstream vStream, fStream;
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);

    if (!vFile) {
        std::cerr << std::strerror(errno) << ": " << vertexPath << std::endl;
    }

    if (!fFile) {
        std::cerr << std::strerror(errno) << ": " << fragmentPath << std::endl;
    }

    // Convert file contents to strings
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    const std::string vStr(vStream.str());
    const std::string fStr(fStream.str());

    int success;
    char infoLog[512];

    const char *vertexShaderSource = vStr.c_str();
    const char *fragmentShaderSource = fStr.c_str();

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Log any errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "[GL Error] Vertex shader compilation failed\n    "
                  << infoLog << std::endl;
        return 0;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Log any errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "[GL Error] Fragment shader compilation failed\n    "
                  << infoLog << std::endl;
        return 0;
    }

    // Compile shaders into program
    GLuint shaderProgram = glCreateProgram();
    shaderPrograms.push_back(shaderProgram);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Log any errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "[GL Error] Shader program linking failed\n    "
                  << infoLog << std::endl;
        return 0;
    }

    // TODO: Optimize this -- do we need to delete every time?
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // return index of location in shaderPrograms array.
    return shaderPrograms.size() - 1;
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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // clang-format on

    setVertices(vertices); // add move assignment?
    addVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    // addVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3 * sizeof(float));
    addVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 3 * sizeof(float));

    // setIndices(indices1);

    createShaderProgram("res/vertex.glsl", "res/fragment.glsl");

    loadTexture2D("res/container.jpg", GL_RGB);
    loadTexture2D("res/awesomeface.png", GL_RGBA);

    glUseProgram(shaderPrograms[0]);

    glUniform1i(glGetUniformLocation(shaderPrograms[0], "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderPrograms[0], "texture2"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glUseProgram(shaderPrograms[0]);
}

void Renderer::renderLoop(float time) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1f(glGetUniformLocation(shaderPrograms[0], "mixValue"), mixValue);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we translate in the opposite direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[0], "model"),
                       1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[0], "view"),
                       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[0], "projection"),
                       1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAOs[0]);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::showWireframe(bool value) {
    glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
}
