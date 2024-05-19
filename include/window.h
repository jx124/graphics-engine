#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <cstdio>

class Window {
public:
    Window(int width, int height, std::string title);
    bool should_close() { return glfwWindowShouldClose(this->ptr); }
    void process_input();

    int width, height;
    std::string title;
    GLFWwindow* ptr;

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void error_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* userParam);
};
