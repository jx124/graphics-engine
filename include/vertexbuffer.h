#pragma once

#include <glad/glad.h>

#include <vector>

class VertexBuffer {
public:
    VertexBuffer() { glGenBuffers(1, &this->id); }
    // create destructor after implementing renderer class
    
    void bind() { glBindBuffer(GL_ARRAY_BUFFER, this->id); }
    void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

    // copies user-defined data into currently bound buffer
    template <typename T>
    void write_buffer_data(const std::vector<T>& buffer, GLenum usage) {
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), buffer.data(), usage);
    }

    GLuint id;
};
