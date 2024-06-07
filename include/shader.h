#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>

#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader {
public:
    Shader(std::string vertex_path, std::string fragment_path);
    
    void use() const { glUseProgram(this->id); };

    void set(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
    };
    void set(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
    };
    void set(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
    };
    void set(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value));
    };
    void set(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value));
    };
    void set(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value));
    };
    void set(const std::string& name, const glm::mat2& value) const {
        glUniformMatrix2fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    };
    void set(const std::string& name, const glm::mat3& value) const {
        glUniformMatrix3fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    };
    void set(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    };

    GLuint id;

    enum Type {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

private:
    GLuint compile(const std::string& source, Shader::Type type);
    void link(GLuint vertex, GLuint fragment);
};
