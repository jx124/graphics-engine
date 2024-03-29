#include "window.h"

float mixValue = 0.0f;

Window::Window(uint32_t width, uint32_t height, const char *windowName)
    : width(width), height(height), windowName(windowName), lastFrame(0.0f) {

    this->renderer = std::make_unique<Renderer>(width, height);
    RendererState &state = *(this->renderer->state);

    state.cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    state.cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    state.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    state.view = glm::lookAt(state.cameraPos, state.cameraPos + state.cameraFront, state.cameraUp);

    state.lastFrameTime = 0.0f;
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::createWindow() {
    // Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create glfw window
    window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    if (window == NULL) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Register resize callback and set current context
    glfwSetFramebufferSizeCallback(window, Window::framebufferSizeCallback);
    glfwMakeContextCurrent(window);

    // Set Renderer instance as window user so it can be accessed in static callbacks
    glfwSetWindowUserPointer(window, this->renderer.get());

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

#ifdef DEBUG
    std::cout << "[DEBUG MODE] OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Window::debugMessageCallback, 0);
#endif

    std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;

    // TODO: handle mouse events only on mouse hold
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, Window::mouseCallback);
    glfwSwapInterval(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, false);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Window::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    size_t newWidth = static_cast<size_t>(width);
    size_t newHeight = static_cast<size_t>(height);

    if (renderer->width != newWidth || renderer->height != newHeight) {
        renderer->width = newWidth;
        renderer->height = newHeight;
        renderer->toResize = true;
    }
}

static bool firstMouse = true;
static float lastX, lastY;
static float sensitivity = 0.2f;
static float pitch, yaw;
static glm::vec3 staticCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

void Window::mouseCallback(GLFWwindow *window, double xPos, double yPos) {
    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureMouse || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        firstMouse = true;
        return;
    }

    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = (xPos - lastX) * sensitivity;
    float yOffset = (lastY - yPos) * sensitivity;
    lastX = xPos;
    lastY = yPos;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 direction;
    direction.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    staticCameraFront = glm::normalize(direction);
}

void Window::processInput(GLFWwindow *window) {
    float currentFrame = glfwGetTime();
    RendererState &state = *(this->renderer->state);

    float deltaTime = currentFrame - state.lastFrameTime;
    state.lastFrameTime = currentFrame;
    state.cameraFront = staticCameraFront;

    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        mixValue += 0.01f;
        if (mixValue > 1.0f) {
            mixValue = 1.0f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mixValue -= 0.01f;
        if (mixValue < 0.0f) {
            mixValue = 0.0f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        state.cameraPos += cameraSpeed * state.cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        state.cameraPos -= cameraSpeed * state.cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        state.cameraPos -= glm::normalize(glm::cross(state.cameraFront, state.cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        state.cameraPos += glm::normalize(glm::cross(state.cameraFront, state.cameraUp)) * cameraSpeed;
    }
    state.view = glm::lookAt(state.cameraPos, state.cameraPos + state.cameraFront, state.cameraUp);
}

void GLAPIENTRY Window::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                             GLsizei length, const GLchar *message, const void *userParam) {

    (void)source;
    (void)id;
    (void)length;
    (void)userParam;
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
        this->renderer->updateLoop(static_cast<float>(glfwGetTime()));

        // Check for events and swap buffers;
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}
