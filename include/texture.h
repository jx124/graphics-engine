#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>

class Texture {
public:
    Texture(const std::string& image_path);

    GLuint id;
    int width, height, n_channels;
    int index; // for setting shader uniforms
    static inline int num_textures = 0;
    std::string type; // diffuse or specular, TODO: use enum
    std::string path;
};
