#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    // Open shader files
    std::ostringstream vStream, fStream;
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);

    if (!vFile) {
        std::cerr << std::strerror(errno) << ": " << vertexPath << std::endl;
        throw std::runtime_error("Failed to open vertex shader file");
    }

    if (!fFile) {
        std::cerr << std::strerror(errno) << ": " << fragmentPath << std::endl;
        throw std::runtime_error("Failed to open fragment shader file");
    }

    // Convert file contents to strings
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    const std::string vStr(vStream.str());
    const std::string fStr(fStream.str());

    int success;
    char infoLog[512];

    const char *vertexShaderSource = vStr.c_str();
    const char *fragmentShaderSource = fStr.c_str();

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Log any errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "[GL Error] Vertex shader compilation failed\n    "
                  << infoLog << std::endl;
        throw std::runtime_error("Failed to compile vertex shader");
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Log any errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "[GL Error] Fragment shader compilation failed\n    "
                  << infoLog << std::endl;
        throw std::runtime_error("Failed to compile fragment shader");
    }

    // Compile shaders into program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    // Log any errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "[GL Error] Shader program linking failed\n    "
                  << infoLog << std::endl;
        throw std::runtime_error("Failed to link shader program");
    }

    // TODO: Optimize this -- do we need to delete every time?
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::bind() {
    glUseProgram(ID);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, GLint value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}