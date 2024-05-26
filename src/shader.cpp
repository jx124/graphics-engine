#include "shader.h"

Shader::Shader(std::string vertex_path, std::string fragment_path) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);

    if (!vertex_file.is_open()) {
        std::perror(("Error opening " + vertex_path).c_str());
        std::terminate();
    }
    if (!fragment_file.is_open()) {
        std::perror(("Error opening " + fragment_path).c_str());
        std::terminate();
    }

    std::stringstream vertex_stream, fragment_stream;

    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();

    if (vertex_stream.bad()) {
        std::perror(("Error reading " + vertex_path).c_str());
        std::terminate();
    }
    if (fragment_stream.bad()) {
        std::perror(("Error reading " + fragment_path).c_str());
        std::terminate();
    }
    
    vertex_file.close();
    fragment_file.close();

    std::string vertex_code = vertex_stream.str();
    std::string fragment_code = fragment_stream.str();

    GLuint vertex_shader = compile(vertex_code, Shader::Type::Vertex);
    GLuint fragment_shader = compile(fragment_code, Shader::Type::Fragment);
    link(vertex_shader, fragment_shader); 

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

GLuint Shader::compile(const std::string& source, Shader::Type type) {
    GLuint shader = glCreateShader(type);
    const char* source_ptr = source.c_str();
    glShaderSource(shader, 1, &source_ptr, nullptr);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        std::cerr << "Error compiling " << (type == Shader::Type::Vertex ? "vertex " : "fragment ")
            << "shader: " << log << std::endl;
        std::terminate();
    }

    return shader;
}

void Shader::link(GLuint vertex, GLuint fragment) {
    this->id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    int success;
    char log[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    
    if (!success) {
        std::cerr << "Error linking shader: " << log << std::endl;
        std::terminate();
    }
}
