#include "window.h"
#include <csignal>

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
    glfwSetWindowUserPointer(window, this); // save a pointer to this Window instance; to be accessed in callbacks

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    this->ptr = window;
    this->state.last_x = width / 2;
    this->state.last_y = height / 2;
};

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Window* win_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win_ptr->width = width;
    win_ptr->height= height;

    glViewport(0, 0, width, height);
}

void Window::process_input() {
    // Close window
    if (glfwGetKey(this->ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(this->ptr, true);
    }
    // Enable wireframe
    if (glfwGetKey(this->ptr, GLFW_KEY_TAB) == GLFW_PRESS && this->state.tab_key_released) {
        this->state.tab_key_released = false;
        this->state.is_wireframe = !this->state.is_wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, this->state.is_wireframe ? GL_LINE : GL_FILL);
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_TAB) == GLFW_RELEASE && !this->state.tab_key_released) {
        this->state.tab_key_released = true;
    }
    // Enable debug menu
    if (glfwGetKey(this->ptr, GLFW_KEY_E) == GLFW_PRESS && this->state.e_key_released) {
        this->state.e_key_released = false;
        this->state.show_debug = !this->state.show_debug;
        this->state.first_mouse = true;
        glfwSetInputMode(this->ptr, GLFW_CURSOR, this->state.show_debug ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_E) == GLFW_RELEASE && !this->state.e_key_released) {
        this->state.e_key_released = true;
    }
    // Change texture mix
    if (glfwGetKey(this->ptr, GLFW_KEY_UP) == GLFW_PRESS) {
        this->state.mix = std::min(this->state.mix + 0.02f, 1.0f);
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_DOWN) == GLFW_PRESS) {
        this->state.mix = std::max(this->state.mix - 0.02f, 0.0f);
    }
    // Camera controls
    float adj_camera_speed = this->state.camera_speed * this->state.delta_time;
    if (glfwGetKey(this->ptr, GLFW_KEY_W) == GLFW_PRESS) {
        this->state.camera_pos += adj_camera_speed * this->state.camera_front;
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_S) == GLFW_PRESS) {
        this->state.camera_pos -= adj_camera_speed * this->state.camera_front;
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_A) == GLFW_PRESS) {
        this->state.camera_pos -= adj_camera_speed * glm::normalize(glm::cross(this->state.camera_front, this->state.camera_up));
    }
    if (glfwGetKey(this->ptr, GLFW_KEY_D) == GLFW_PRESS) {
        this->state.camera_pos += adj_camera_speed * glm::normalize(glm::cross(this->state.camera_front, this->state.camera_up));
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

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    std::fprintf(stderr, "[OpenGL] %s: %s. Severity: %s. Message: %s\n",
            type_str.c_str(), source_str.c_str(), severity_str.c_str(), message);
    glFinish();
    raise(SIGTRAP);
}

void Window::mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    // get pointer to current Window instance
    Window* win_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (win_ptr->state.show_debug) {
        return;
    }

    if (win_ptr->state.first_mouse) {
        win_ptr->state.last_x = x_pos;
        win_ptr->state.last_y = y_pos;
        win_ptr->state.first_mouse = false;
    }

    float x_offset = x_pos - win_ptr->state.last_x;
    float y_offset = win_ptr->state.last_y - y_pos;

    win_ptr->state.last_x = x_pos;
    win_ptr->state.last_y = y_pos;

    x_offset *= win_ptr->state.camera_sensitivity;
    y_offset *= win_ptr->state.camera_sensitivity;

    float &yaw = win_ptr->state.yaw;
    float &pitch = win_ptr->state.pitch;

    yaw += x_offset;
    pitch += y_offset;

    pitch = std::clamp(pitch, -89.9f, 89.9f);

    glm::vec3 direction(std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
                        std::sin(glm::radians(pitch)),
                        std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch)));
    win_ptr->state.camera_front = glm::normalize(direction);
}

void Window::scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    (void) x_offset;
    // get pointer to current Window instance
    Window* win_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));

    win_ptr->state.fov -= static_cast<float>(y_offset);
    win_ptr->state.fov = std::clamp(win_ptr->state.fov, 1.0f, 45.0f);
}
