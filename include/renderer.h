#pragma once

#include "window.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "elementbuffer.h"
#include "shader.h"
#include "texture.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

struct Object {
    VertexArray vao;
    Shader shader;
};

class Renderer {
public:
    Renderer(Window* window);
    
    void init();
    void update();
    void render();
    void render_ui();

private:
    Window* window;
    std::vector<Object> objects{};
};
