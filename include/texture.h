#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <vector>
#include <future>
#include <thread>

struct ImageData {
    unsigned char* data;
    int width, height, n_channels;
    std::string path;
};

class Texture {
public:
    Texture(const std::string& image_path);
    Texture(const ImageData& image_data);

    static std::vector<Texture> load_textures(std::vector<std::string> image_paths);

    GLuint id;
    int width, height, n_channels;
    int index; // for setting shader uniforms
    static inline int num_textures = 0;
    std::string type; // diffuse or specular, TODO: use enum
    std::string path;
};
