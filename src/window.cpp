#include "window.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

Window::Window(uint32_t width, uint32_t height, const char *windowName)
    : width(width), height(height), windowName(windowName) {
    
    this->renderer = std::make_unique<Renderer>();
}

Window::~Window() {
    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    glfwTerminate();
}

bool Window::createWindow() {
    // Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create glfw window
    window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        return false; // TODO: use an enum instead?
    }

    // Register resize callback and set current context
    glfwSetFramebufferSizeCallback(window, Window::framebufferSizeCallback);
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

#ifdef DEBUG
    std::cout << "[DEBUG MODE] OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Window::debugMessageCallback, 0);
#endif

    return true;
}
/*
void Window::setVertices(std::vector<float> vertices) {
    this->vertices = vertices;

    // Generate and bind vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    VAOs.push_back(VAO);

    // Generate and bind vertex buffer object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    VBOs.push_back(VBO);

    // Copy data into buffer memory
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);
}

void Window::addVertexAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset) {
    glVertexAttribPointer(index, size, type, normalized, stride, (GLvoid *)offset);
    glEnableVertexAttribArray(index);
}

void Window::setIndices(std::vector<uint32_t> indices) {
    this->indices = indices;

    // Generate and bind element buffer object
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    EBOs.push_back(EBO);

    // Copy data into buffer memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), this->indices.data(), GL_STATIC_DRAW);
}

void Window::createShaderProgram(const char *vertexPath, const char *fragmentPath) {
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

    vertexShaderSource = vStr.c_str();
    fragmentShaderSource = fStr.c_str();

    // Compile vertex shader
    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Log any errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "[GL Error] Vertex shader compilation failed\n    "
                  << infoLog << std::endl;
        return;
    }

    // Compile fragment shader
    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Log any errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "[GL Error] Fragment shader compilation failed\n    "
                  << infoLog << std::endl;
        return;
    }

    // Compile shaders into program
    shaderProgram = glCreateProgram();
    shaders.push_back(shaderProgram);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Log any errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "[GL Error] Shader program linking failed\n    "
                  << infoLog << std::endl;
        return;
    }

    // TODO: Optimize this -- do we need to delete every time?
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Window::loadShader(const char *filePath, Window::ShaderType type) {
    std::ostringstream sstream;
    std::ifstream shaderFile(filePath);

    if (!shaderFile) {
        std::cerr << std::strerror(errno) << ": " << filePath << std::endl;
    }

    sstream << shaderFile.rdbuf();
    const std::string str(sstream.str());

    int success;
    char infoLog[512];

    switch (type) {
        case ShaderType::Vertex: {
            // Compile shader
            vertexShaderSource = str.c_str();
            this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);

            // Log any errors
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cerr << "[GL Error] Vertex shader compilation failed\n    "
                          << infoLog << std::endl;
            }
            break;
        }
        case ShaderType::Fragment: {
            fragmentShaderSource = str.c_str();
            this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);

            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cerr << "[GL Error] Fragment shader compilation failed\n    "
                          << infoLog << std::endl;
            }
            break;
        }
        default:
            break;
    }
}

uint32_t Window::loadTexture(const char *filePath, GLenum textureUnit, GLint format) {
    stbi_set_flip_vertically_on_load(true);

    int width, height, nChannels;
    unsigned char *data = stbi_load(filePath, &width, &height, &nChannels, 0);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "[GL Error] Failed to load texture" << std::endl;
        return 0;
    }
    stbi_image_free(data);

    return texture;
}

void Window::compileShaders() {
    // Compile shaders into program
    shaderProgram = glCreateProgram();
    shaders.push_back(shaderProgram);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];

    // Log any errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "[GL Error] Shader program linking failed\n    "
                  << infoLog << std::endl;
        return;
    }
}
*/

void Window::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void GLAPIENTRY Window::debugMessageCallback(GLenum source,
                                             GLenum type,
                                             GLuint id,
                                             GLenum severity,
                                             GLsizei length,
                                             const GLchar *message,
                                             const void *userParam) {

    fprintf(stderr, "%s type: 0x%x, severity: 0x%x, message: %s\n",
            type == GL_DEBUG_TYPE_ERROR ? "[GL Error]" : "[GL Other]",
            type, severity, message);
}

void Window::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t i = 0; i < VAOs.size(); i++) {
        glUseProgram(shaders[i]);
        glUniform1i(glGetUniformLocation(shaders[i], "texture1"), 0);
        glUniform1i(glGetUniformLocation(shaders[i], "texture2"), 1);
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void Window::showWireframe(bool value) {
    glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
}

void Window::run() {
    this->renderer->renderInit();

    while (!glfwWindowShouldClose(window)) {
        // Process key inputs
        processInput(window);

        // Run render commands
        this->renderer->renderLoop();

        // Check for events and swap buffers;
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}
