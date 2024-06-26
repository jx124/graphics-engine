#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <cstdio>
#include <algorithm>

struct WindowState {
    bool is_wireframe = false;
    bool tab_key_released = true;
    bool show_debug = false;
    bool e_key_released = true;
    bool first_mouse = true;
    float mix = 0.0f;

    glm::vec3 camera_pos = {0.0f, 0.0f, 3.0f};
    glm::vec3 camera_front = {0.0f, 0.0f, -1.0f};
    glm::vec3 camera_up = {0.0f, 1.0f, 0.0f};
    float camera_speed = 5.0f;
    float camera_sensitivity = 0.1f;
    float pitch = 0.0f;
    float yaw = -90.0f;
    float fov = 45.0f;

    float prev_time = 0.0f;
    float curr_time = 0.0f;
    float delta_time = 0.0f;

    int last_x;
    int last_y;

    glm::vec3 dirlight_ambient = {0.05f, 0.05f, 0.05f};
    glm::vec3 dirlight_diffuse = {0.4f, 0.4f, 0.4f};
    glm::vec3 dirlight_specular = {0.5f, 0.5f, 0.5f};

    glm::vec3 pointlight_ambient = {0.05f, 0.05f, 0.05f};
    glm::vec3 pointlight_diffuse = {0.8f, 0.8f, 0.8f};
    glm::vec3 pointlight_specular = {1.0f, 1.0f, 1.0f};

    glm::vec3 spotlight_ambient = {0.0f, 0.0f, 0.0f};
    glm::vec3 spotlight_diffuse = {1.0f, 1.0f, 1.0f};
    glm::vec3 spotlight_specular = {1.0f, 1.0f, 1.0f};
    float cutoff = 12.5f;
    float outer_cutoff = 17.5f;

    float shininess = 32.0f;
};

class Window {
public:
    Window(int width, int height, std::string title);
    bool should_close() { return glfwWindowShouldClose(this->ptr); }
    void process_input();

    int width, height;
    std::string title;
    GLFWwindow* ptr;
    WindowState state;

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void error_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* userParam);
    static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
    static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
};
