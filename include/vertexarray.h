#pragma once

#include <glad/glad.h>

class VertexArray {
public:
    VertexArray() { glGenVertexArrays(1, &this->id); }
    // create destructor after implementing renderer class

    void bind() const { glBindVertexArray(this->id); }
    void unbind() const { glBindVertexArray(0); }

    GLuint id;
};
