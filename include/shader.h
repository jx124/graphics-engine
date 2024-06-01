#pragma once

#include <glad/glad.h>

#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader {
public:
    Shader(std::string vertex_path, std::string fragment_path);
    
    void use() { glUseProgram(this->id); };

    void set(const std::string& name, bool value) { glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value); };
    void set(const std::string& name, int value) { glUniform1i(glGetUniformLocation(this->id, name.c_str()), value); };
    void set(const std::string& name, float value) { glUniform1f(glGetUniformLocation(this->id, name.c_str()), value); };

    GLuint id;

    enum Type {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

private:
    GLuint compile(const std::string& source, Shader::Type type);
    void link(GLuint vertex, GLuint fragment);
};
