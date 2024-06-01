#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>

class Texture {
public:
    Texture(const std::string& image_path, GLenum format);

    GLuint id;
    int width, height, n_channels;
    int index; // for setting shader uniforms
    static inline int num_textures = 0;
};
