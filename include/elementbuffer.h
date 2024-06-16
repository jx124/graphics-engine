#pragma once

#include <glad/glad.h>

#include <vector>

class ElementBuffer {
public:
    ElementBuffer() { glGenBuffers(1, &this->id); }
    // create destructor after implementing renderer class
    
    void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id); }
    // do not unbind EBO while VAO is still active
    void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

    // copies user-defined data into currently bound buffer
    template <typename T>
    void write_buffer_data(const std::vector<T>& buffer, GLenum usage) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size() * sizeof(GLuint), buffer.data(), usage);
    }

    GLuint id;
};
