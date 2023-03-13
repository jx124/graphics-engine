#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

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
    GLuint VAO, VBO, EBO, vertexShader, fragmentShader, shaderProgram;
    const char *vertexShaderSource;
    const char *fragmentShaderSource;

public:
    Window(uint32_t width, uint32_t height, const char *windowName);
    ~Window();

    enum ShaderType {
        Vertex,
        Fragment
    };

    bool createWindow();
    void setVertices(std::vector<float> vertices);
    void setIndices(std::vector<uint32_t> indices);
    void loadShader(const char *filePath, Window::ShaderType type);
    void compileShaders();
    void render();
    void run();
};

Window::Window(uint32_t width, uint32_t height, const char *windowName)
    : width(width), height(height), windowName(windowName) {
}

Window::~Window() {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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

void Window::setVertices(std::vector<float> vertices) {
    this->vertices = vertices;

    // Generate and bind vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind vertex buffer object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy data into buffer memory
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

    // Set vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void Window::setIndices(std::vector<uint32_t> indices) {
    this->indices = indices;

    // Generate and bind element buffer object
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Copy data into buffer memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), this->indices.data(), GL_STATIC_DRAW);
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

void Window::compileShaders() {
    // Compile shaders into program
    shaderProgram = glCreateProgram();
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

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
}

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
            type == GL_DEBUG_TYPE_ERROR ? "[GL Error]" : "",
            type, severity, message);
}

void Window::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Window::run() {
    while (!glfwWindowShouldClose(window)) {
        // Process key inputs
        processInput(window);

        // Run render commands
        render();

        // Check for events and swap buffers;
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}