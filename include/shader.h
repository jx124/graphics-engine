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

    void set(const std::string& name, bool value);
    void set(const std::string& name, int value);
    void set(const std::string& name, float value);

    GLuint id;

    enum Type {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

private:
    GLuint compile(const std::string& source, Shader::Type type);
    void link(GLuint vertex, GLuint fragment);
};
