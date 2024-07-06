#include "framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
    : width(width), height(height), colorbuffer(width, height) {
    glGenFramebuffers(1, &this->id);
    this->bind();

    // Attach empty texture to current framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorbuffer.id, 0);

    // Create a renderbuffer object with both depth and stencil attachments
    glGenRenderbuffers(1, &this->renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

     // Attach renderbuffer object to depth and stencil attachment of framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[OpenGL] Framebuffer error: framebuffer is not complete." << std::endl;
        this->unbind();
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Create quad that fills the whole screen in NDC
    constexpr float quad_vertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    GLuint quad_vertexbuffer;
    glGenVertexArrays(1, &this->quad_vertexarray);
    glGenBuffers(1, &quad_vertexbuffer);

    glBindVertexArray(this->quad_vertexarray);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    this->screen_shader = Shader("assets/shaders/framebuffer_vertex.glsl", "assets/shaders/framebuffer_fragment.glsl");
    this->screen_shader.use();
    this->screen_shader.set("screenTexture", this->colorbuffer.unit);
}

void Framebuffer::draw_to_screen() {
    glActiveTexture(GL_TEXTURE0 + this->colorbuffer.unit);
    glBindTexture(GL_TEXTURE_2D, this->colorbuffer.id);
    this->screen_shader.use();
    glBindVertexArray(this->quad_vertexarray);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
