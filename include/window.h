#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "renderer.h"

class Window {
private:
    uint32_t width, height;
    const char *windowName;
    GLFWwindow *window;

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void mouseCallback(GLFWwindow *window, double xPos, double yPos);
    void processInput(GLFWwindow *window);
    static void GLAPIENTRY debugMessageCallback(GLenum source,
                                                GLenum type,
                                                GLuint id,
                                                GLenum severity,
                                                GLsizei length,
                                                const GLchar *message,
                                                const void *userParam);
    std::unique_ptr<Renderer> renderer;
    glm::vec3 cameraPos, cameraFront, cameraUp;
    glm::mat4 view;
    float lastFrame;

public:
    Window(uint32_t width, uint32_t height, const char *windowName);
    ~Window();

    bool createWindow();
    void showWireframe(bool value);
    void run();
};
