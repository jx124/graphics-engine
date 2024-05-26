#pragma once

#include <glad/glad.h>

class VertexArray {
public:
    VertexArray() { glGenVertexArrays(1, &this->id); }
    // create destructor after implementing renderer class

    void bind() { glBindVertexArray(this->id); }
    void unbind() { glBindVertexArray(0); }

    GLuint id;
};
