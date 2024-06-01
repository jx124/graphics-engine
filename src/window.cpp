#include "window.h"

Window::Window(int width, int height, std::string title) : width(width), height(height), title(title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        std::terminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        std::terminate();
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    std::cout << "[OpenGL] Version: " << glGetString(GL_VERSION) << std::endl;
    glDebugMessageCallback(error_message_callback, nullptr);
#endif

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    this->ptr = window;
};

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void) window;
    glViewport(0, 0, width, height);
}

void Window::process_input() {
    if (glfwGetKey(this->ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(this->ptr, true);
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_TAB) == GLFW_PRESS && this->state.key_released) {
        this->state.key_released = false;
        this->state.is_wireframe = !this->state.is_wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, this->state.is_wireframe ? GL_LINE : GL_FILL);
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_TAB) == GLFW_RELEASE && !this->state.key_released) {
        this->state.key_released = true;
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_UP) == GLFW_PRESS) {
        this->state.mix = std::min(this->state.mix + 0.02f, 1.0f);
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_DOWN) == GLFW_PRESS) {
        this->state.mix = std::max(this->state.mix - 0.02f, 0.0f);
    }
}

void Window::error_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam) {
    std::string source_str, type_str, severity_str;
    (void) id, (void) length, (void) userParam;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            source_str = "OpenGL API call";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_str = "Window-system API call";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_str = "Shader compilation";
            break;
        default:
            source_str = "Other";
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_str = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_str = "Deprecated behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_str = "Undefined behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_str = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_str = "Performance";
            break;
        default:
            type_str = "Other";
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            severity_str = "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_str = "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severity_str = "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_str = "Notification";
            break;
    }

    std::fprintf(stderr, "[OpenGL] %s: %s. Severity: %s. Message: %s\n",
            type_str.c_str(), source_str.c_str(), severity_str.c_str(), message);
}
