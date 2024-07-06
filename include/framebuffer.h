#pragma once

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"

class Framebuffer {
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);

    void bind() { glBindFramebuffer(GL_FRAMEBUFFER, this->id); }
    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void draw_to_screen();

    GLuint id;
    GLuint renderbuffer;
    GLuint quad_vertexarray;
    int width, height;

    Texture colorbuffer;
    Shader screen_shader;
};