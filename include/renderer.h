#pragma once

#include "model.h"
#include "window.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "elementbuffer.h"
#include "shader.h"
#include "texture.h"
#include "framebuffer.h"
#include "cubemap.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

struct Entity {
    size_t shader_id;
    size_t model_id;
    size_t transform_id;
};

using Transform = glm::mat4;

class Renderer {
public:
    Renderer(Window* window);
    
    void init();
    void update();
    void render();
    void render_ui();

private:
    Window* window;
    std::vector<Shader> shaders;
    std::vector<Model> models;
    std::vector<Transform> transforms;

    std::vector<Entity> entities;
    std::map<float, Entity> transparent_entities; // sorted map with key = distance

    Framebuffer framebuffer;
    CubeMap skybox;
};
