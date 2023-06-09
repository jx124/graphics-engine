#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>

class Shader {
public:
    GLuint ID;
    
    Shader(const char *vertexPath, const char *fragmentPath);
    void bind();
    void unbind();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, GLint value) const;
    void setFloat(const std::string &name, GLfloat value) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};