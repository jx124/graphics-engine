#include "window.h"

Window::Window(uint32_t width, uint32_t height, const char *windowName)
    : width(width), height(height), windowName(windowName) {

    this->renderer = std::make_unique<Renderer>();
}

Window::~Window() {
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
