#include "window.h"

float mixValue = 0.0f;

Window::Window(uint32_t width, uint32_t height, const char *windowName)
    : width(width), height(height), windowName(windowName), lastFrame(0.0f) {

    this->renderer = std::make_unique<Renderer>();
    this->cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    this->view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

#ifdef DEBUG
    std::cout << "[DEBUG MODE] OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Window::debugMessageCallback, 0);
#endif

    // TODO: handle mouse events only on mouse hold
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, Window::mouseCallback);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    this->io = std::make_unique<ImGuiIO>(ImGui::GetIO());
    io->AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, false);
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Window::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
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
    float deltaTime = currentFrame - this->lastFrame;
    this->lastFrame = currentFrame;
    this->cameraFront = staticCameraFront;

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
        this->cameraPos += cameraSpeed * this->cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->cameraPos -= cameraSpeed * this->cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->cameraPos -= glm::normalize(glm::cross(this->cameraFront, this->cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->cameraPos += glm::normalize(glm::cross(this->cameraFront, this->cameraUp)) * cameraSpeed;
    }
    this->view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
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
        this->renderer->setViewMatrix(this->view);
        this->renderer->renderLoop(static_cast<float>(glfwGetTime()));
        this->renderer->renderImGui(*io);

        // Check for events and swap buffers;
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}
